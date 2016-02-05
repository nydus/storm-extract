/*****************************************************************************/
/* storm-extract.cpp                         Copyright 2016 Justin J. Novack */
/*---------------------------------------------------------------------------*/
/* list and extract files from the Heroes of the Storm CASC archives         */
/*****************************************************************************/

#include <CascLib.h>
#include <SimpleOpt.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
//#include <regex>

using namespace std;

string version = "0.1.0"

struct tSearchResult {
    string strFileName;
    string strFullPath;
};

// Valid options
enum {
    OPT_HELP,
    OPT_PATTERN,
    OPT_EXTRACT,
    OPT_DEST,
    // OPT_REGEX,
    OPT_FILEEXT,
    OPT_FULLPATH,
    OPT_LOWERCASE,
    OPT_VERBOSE
};

bool bVerbose = false;

const CSimpleOpt::SOption COMMAND_LINE_OPTIONS[] = {
    { OPT_HELP,             "-h",               SO_NONE    },
    { OPT_HELP,             "--help",           SO_NONE    },
    { OPT_VERBOSE,          "-v",               SO_NONE    },
    { OPT_VERBOSE,          "--verbose",        SO_NONE    },
    { OPT_EXTRACT,          "-x",               SO_REQ_SEP },
    { OPT_EXTRACT,          "--extract",        SO_REQ_SEP },
    { OPT_DEST,             "-o",               SO_REQ_SEP },
    { OPT_DEST,             "--dest",           SO_REQ_SEP },
    { OPT_FULLPATH,         "-f",               SO_NONE    },
    { OPT_FULLPATH,         "--fullpath",       SO_NONE    },
    { OPT_LOWERCASE,        "-c",               SO_NONE    },
    { OPT_LOWERCASE,        "--lowercase",      SO_NONE    },
    // { OPT_REGEX,            "-r",               SO_REQ_SEP },
    // { OPT_REGEX,            "--regex",          SO_REQ_SEP },
    { OPT_PATTERN,          "-p",               SO_REQ_SEP },
    { OPT_PATTERN,          "--pattern",        SO_REQ_SEP },
    { OPT_FILEEXT,          "-e",               SO_REQ_SEP },
    { OPT_FILEEXT,          "--extension",      SO_REQ_SEP },

    SO_END_OF_OPTIONS
};

void showUsage(const std::string &pathToExecutable) {
    cout << "storm-extract v" << version << endl
         << "Usage: " << pathToExecutable << " [options] <CASC_ROOT> <PATTERN>" << endl
         << endl
         << "This program can list and optionally extract files from a CASC storage container." << endl
         << endl
         << "Options:" << endl
         << "    -h, --help                Display this help" << endl
         << "    -v, --verbose             Prints actions taken" << endl
         << "    -p, --pattern <STRING>    Search for filenames matching STRING" << endl
         << "    -e, --extension <STRING>  Search for filenames having extension STRING" << endl
         << "    -x, --extract             Extract the files found" << endl
         << "    -o, --dest <PATH>         The folder where the files are extracted (extract only)" << endl
         << "                                (default: current working directory)" << endl
         << "    -f, --fullpath            During extraction, preserve the path hierarchy found" << endl
         << "                                inside the storage (extract only)" << endl
         << "    -c, --lowercase           Convert extracted file paths to lowercase (extract only)" <<endl
         << endl
         << "Examples:" << endl
         << endl
         << "  1) List all files in CASC storage container:" << endl
         << endl
         << "       ./storm-extract \"/Applications/Heroes of the Storm/\" /" << endl
         << endl
         << "  2) Extract a specific file:" << endl
         // << "       IMPORTANT: The file name must be enclosed in \"\" to prevent the shell to" << endl
         // << "                  interpret the \\ character as the start of an escape sequence." << endl
         << endl
         << "       ./storm-extract -o out \"/Applications/Heroes of the Storm/\" \"Path/To/The/File\"" << endl
         << endl
         // << "  3) Extract some specific files, preserving the path hierarchy:" << endl
         // << endl
         // << "       ./storm-extract -f -o out -l listfile-wow6.txt \"/Applications/Heroes of the Storm/\" \"Path\\To\\Extract\\*\"" << endl
         << endl
         << "Copyright(c) 2016 Justin J. Novack" << endl
         << "https://www.github.com/jnovack/storm-extract" << endl;
}

// Overloaded echo command.
void echo() {
    if (bVerbose) {
        cout << endl;
    }
}

void echo(const std::string &output) {
    if (bVerbose) {
        cout << output;
    }
}

bool hasExtension(const std::string filename, const std::string extension) {
    // http://stackoverflow.com/questions/874134/find-if-string-ends-with-another-string-in-c
    return (
        // Prevent exception of type std::out_of_range: basic_string because the file extension is greater than the filename
        (filename.length() > extension.length()) &&
        // http://stackoverflow.com/questions/874134/find-if-string-ends-with-another-string-in-c
        (0 == filename.compare(filename.length() - extension.length(), extension.length(), extension))
    );
}

int main(int argc, char** argv) {
    HANDLE hStorage;
    string strSearchPattern;
    string strFilePattern;
    string strFileExt;
    string strStorage;
    string strDestination = ".";
    vector<tSearchResult> searchResults;
    bool bUseFullPath = false;
    bool bLowerCase = false;
    bool bExtract = false;
    bool bPattern = false;
    bool bSearchPattern = false;
    bool bFileExt = false;
    int matches = 0;
    // bool bRegex = false;
    // regex reSearchPattern;


    // Parse the command-line parameters
    CSimpleOpt args(argc, argv, COMMAND_LINE_OPTIONS);
    while (args.Next())
    {
        if (args.LastError() == SO_SUCCESS)
        {
            switch (args.OptionId())
            {
                case OPT_HELP:
                    showUsage(argv[0]);
                    return 0;

                case OPT_DEST:
                    strDestination = args.OptionArg();
                    break;

                case OPT_PATTERN:
                    bPattern = true;
                    bSearchPattern = true;
                    strFilePattern = args.OptionArg();
                    break;

                case OPT_FILEEXT:
                    bFileExt = true;
                    strFileExt = args.OptionArg();
                    break;

                case OPT_FULLPATH:
                    bUseFullPath = true;
                    break;

                case OPT_LOWERCASE:
                    bLowerCase = true;
                    break;

                case OPT_VERBOSE:
                    bVerbose = true;
                    break;

                case OPT_EXTRACT:
                    bExtract = true;
                    break;

                // case OPT_REGEX:
                //     bSearchPattern = true;
                //     bRegex = true;
                //     regex reSearchPattern(args.OptionArg()); //, regex_constants::icase | regex_constants::nosubs );
                //     break;
            }
        }
        else
        {
            cerr << "Invalid argument: " << args.OptionText() << endl;
            return -1;
        }
    }

    if (args.FileCount() != 2) {
        cerr << "Usage: " << argv[0] << " [options] <CASC_ROOT> <PATTERN> [-p pattern]" << endl;
        return -1;
    }


    // Save arguments
    strStorage = args.File(0);
    strSearchPattern = args.File(1);

    // Remove trailing slashes at the end of the storage path (CascLib doesn't like that)
    if ((strStorage[strStorage.size() - 1] == '/') || (strStorage[strStorage.size() - 1] == '\\'))
        strStorage = strStorage.substr(0, strStorage.size() - 1);

    // Open CASC Files
    if (!CascOpenStorage(strStorage.c_str(), 0, &hStorage)) {
        cerr << "Failed to open the storage '" << strStorage << "'" << endl;
        return -1;
    }

    // Explain what we want to do
    echo("Searching for files with: \n");
    echo("* full paths matching '" + strSearchPattern + "'\n");
    if (bPattern) {
        echo("* filenames matching '" + strFilePattern + "'\n");
    }
    if (bFileExt) {
        echo("* extensions matching '" + strFileExt + "'\n");
    }

    // Set up structure
    CASC_FIND_DATA findData;
    HANDLE handle = CascFindFirstFile(hStorage, "*", &findData, NULL);

    if (handle) {
        echo();
        echo("Found files:\n\n");

        do {
            tSearchResult r;
            r.strFileName = findData.szPlainName;
            r.strFullPath = findData.szFileName;

            if (r.strFullPath.find(strSearchPattern) != std::string::npos) {
                if (
                    // No file pattern, No file type
                    (!bPattern && !bFileExt) ||
                    // No file pattern, Yes file type AND file type match.
                    (!bPattern && bFileExt && hasExtension(r.strFileName, strFileExt)) ||
                    // File name contains search pattern, No file type
                    (bPattern && r.strFileName.find(strFilePattern) != std::string::npos && !bFileExt) ||
                    // File name contains search pattern AND matches file type
                    (bPattern && r.strFileName.find(strFilePattern) != std::string::npos && bFileExt &&
                        hasExtension(r.strFileName, strFileExt))
                ){
                    echo(findData.szFileName);
                    echo();
                    matches++;
                    searchResults.push_back(r);
                }
            }
        } while (CascFindNextFile(handle, &findData) && findData.szPlainName);

        CascFindClose(handle);
        echo();
        echo(to_string(matches) + " matches...\n");
    } else {
        echo("No files found!\n");
    }

    // Extraction
    if (bExtract && !searchResults.empty())
    {
        char buffer[1000000];

        cout << endl;
        cout << "Extracting files..." << endl;
        cout << endl;

        if (strDestination.at(strDestination.size() - 1) != '/')
            strDestination += "/";

        vector<tSearchResult>::iterator iter, iterEnd;
        for (iter = searchResults.begin(), iterEnd = searchResults.end(); iter != iterEnd; ++iter)
        {
            string strDestName = strDestination;

            if (bUseFullPath)
            {
                if (bLowerCase){
                    transform(iter->strFullPath.begin(), iter->strFullPath.end(), iter->strFullPath.begin(), ::tolower);
                }

                strDestName += iter->strFullPath;

                size_t offset = strDestName.find("\\");
                while (offset != string::npos)
                {
                    strDestName = strDestName.substr(0, offset) + "/" + strDestName.substr(offset + 1);
                    offset = strDestName.find("\\");
                }

                offset = strDestName.find_last_of("/");
                if (offset != string::npos)
                {
                    string dest = strDestName.substr(0, offset + 1);

                    size_t start = dest.find("/", 0);
                    while (start != string::npos)
                    {
                        string dirname = dest.substr(0, start);

                        DIR* d = opendir(dirname.c_str());
                        if (!d)
                            mkdir(dirname.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
                        else
                            closedir(d);

                        start = dest.find("/", start + 1);
                    }
                }
            }
            else
            {
                if (bLowerCase){
                    transform(iter->strFileName.begin(), iter->strFileName.end(), iter->strFileName.begin(), ::tolower);
                }

                strDestName += iter->strFileName;
            }

            HANDLE hFile;
            if (CascOpenFile(hStorage, iter->strFullPath.c_str(), CASC_LOCALE_ALL, 0, &hFile))
            {
                DWORD read;
                FILE* dest = fopen(strDestName.c_str(), "wb");
                if (dest)
                {
                    do {
                        if (CascReadFile(hFile, &buffer, 1000000, &read))
                            fwrite(&buffer, read, 1, dest);
                    } while (read > 0);

                    fclose(dest);
                }
                else
                {
                    cerr << "Failed to extract the file '" << iter->strFullPath << "' in " << strDestName << endl;
                }

                CascCloseFile(hFile);
            }
            else
            {
                cerr << "Failed to extract the file '" << iter->strFullPath << "' in " << strDestName << endl;
            }
        }
    }

    CascCloseStorage(hStorage);

    return 0;
}
