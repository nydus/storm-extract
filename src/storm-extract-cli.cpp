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
#include <set>
//#include <regex>

using namespace std;

string version = "1.0.3";

struct tSearchResult {
    string strFileName;
    string strFullPath;
};

// Valid options
enum {
    OPT_HELP,
    OPT_VERBOSE,
    OPT_QUIET,
    OPT_EXTRACT,
    OPT_SRC,
    OPT_DEST,
    OPT_FULLPATH,
    OPT_FILEEXT,
    OPT_FILEPTRN,
    OPT_SEARCH,
    OPT_LOWERCASE,
    OPT_LISTDIRS,
    OPT_REGEX
};

bool bVerbose = false;      // Print extra information for logging
bool bQuiet = false;        // Do not print anything.

const CSimpleOpt::SOption COMMAND_LINE_OPTIONS[] = {
    { OPT_HELP,             "-h",               SO_NONE    },
    { OPT_HELP,             "--help",           SO_NONE    },
    { OPT_VERBOSE,          "-v",               SO_NONE    },
    { OPT_VERBOSE,          "--verbose",        SO_NONE    },
    { OPT_QUIET,            "-q",               SO_NONE    },
    { OPT_QUIET,            "--quiet",          SO_NONE    },
    { OPT_EXTRACT,          "-x",               SO_NONE    },
    { OPT_EXTRACT,          "--extract",        SO_NONE    },
    { OPT_SRC,              "-i",               SO_REQ_SEP },
    { OPT_SRC,              "--in",             SO_REQ_SEP },
    { OPT_DEST,             "-o",               SO_REQ_SEP },
    { OPT_DEST,             "--out",            SO_REQ_SEP },
    { OPT_FULLPATH,         "-p",               SO_NONE    },
    { OPT_FULLPATH,         "--path",           SO_NONE    },
    { OPT_LOWERCASE,        "-c",               SO_NONE    },
    { OPT_LOWERCASE,        "--lowercase",      SO_NONE    },
    { OPT_FILEPTRN,         "-f",               SO_REQ_SEP },
    { OPT_FILEPTRN,         "--filename",       SO_REQ_SEP },
    { OPT_FILEEXT,          "-e",               SO_REQ_SEP },
    { OPT_FILEEXT,          "--extension",      SO_REQ_SEP },
    { OPT_SEARCH,           "-s",               SO_REQ_SEP },
    { OPT_SEARCH,           "--search",         SO_REQ_SEP },
    { OPT_LISTDIRS,         "-d",               SO_NONE    },
    { OPT_LISTDIRS,         "--directories",    SO_NONE    },
    // { OPT_REGEX,            "-r",               SO_REQ_SEP },
    // { OPT_REGEX,            "--regex",          SO_REQ_SEP },

    SO_END_OF_OPTIONS
};

void showUsage(const std::string &pathToExecutable) {
    cout << "storm-extract v" << version << endl
         << "  Usage: " << pathToExecutable << " [options]" << endl
         << endl
         << "This program can list and optionally extract files from a Heroes of the Storm CASC storage container." << endl
         << endl
         << "    -h, --help                Display this help" << endl
         << endl
         << "Options:" << endl
         << "  Common:" << endl
         << "    -i, --in <PATH>           Directory where '/HeroesData' is" << endl
         << "                                (default: '/Applications/Heroes of the Storm')" << endl
         << "    -v, --verbose             Prints more information" << endl
         << "    -q, --quiet               Prints nothing, nada, zip" << endl
         << "    -s, --search <STRING>     Restrict results to full paths matching STRING" << endl
         << "    -f, --filename <STRING>   Search for filenames matching STRING" << endl
         << "    -e, --extension <STRING>  Search for filenames having extension STRING" << endl
         // << "    --exclude <ARG1> <ARGN>   Exclude any number of strings" << endl
         << endl
         << "  Search:     storm-extract [options]" << endl
         << endl
         << "  Extract:    storm-extract -x [options]" << endl
         << "    -x, --extract             Extract the files found" << endl
         << "    -o, --out <PATH>          The folder where the files are extracted (extract only)" << endl
         << "                                (default: current working directory)" << endl
         << "    -p, --path                During extraction, preserve the path hierarchy found" << endl
         << "                                inside the storage (extract only)" << endl
         << "    -c, --lowercase           Convert extracted file paths to lowercase (extract only)" <<endl
         << endl
         << "  Directory:  storm-extract -d [options]" << endl
         << "    -d, --directories         Print all directories found" << endl
         << endl
         << "Examples:" << endl
         << endl
         << "  1) List all files in CASC storage container (this will take a while):" << endl
         << endl
         << "       ./storm-extract -i \"/Applications/Heroes of the Storm/\" -f ." << endl
         << endl
         << "  2) Extract a specific file:" << endl
         << endl
         << "       ./storm-extract -i \"/Applications/Heroes of the Storm/\" -f \"path/to/the/file\" -o out" << endl
         << endl
         << "  3) Extract specific filenames (preserving heirarchy):" << endl
         << endl
         << "       ./storm-extract -i \"/Applications/Heroes of the Storm/\" -f GameData.xml -o out -p -x" << endl
         << endl
         << "  4) Extract all English sounds (preserving heirarchy):" << endl
         << endl
         << "       ./storm-extract -i \"/Applications/Heroes of the Storm/\" -s enus -o out -e wav -p -x" << endl
         << "       ./storm-extract -i \"/Applications/Heroes of the Storm/\" -s enus -o out -e ogg -p -x" << endl
         << endl
         << "Copyright(c) 2016 Justin J. Novack" << endl
         << "https://www.github.com/jnovack/storm-extract" << endl;
}

// Overloaded echo command.
void echo() {
    if (!bQuiet) {
          cout << endl;
    }
}

void echo(const std::string &output) {
    if (!bQuiet) {
        cout << output;
    }
}

// Overloaded verbose command.
void verbose() {
    if (!bQuiet && bVerbose) {
          cout << endl;
    }
}

void verbose(const std::string &output) {
    if (!bQuiet && bVerbose) {
        cout << output;
    }
}

// Find out if the end of the string matches another string
bool hasExtension(const std::string filename, const std::string extension) {
    // http://stackoverflow.com/questions/874134/find-if-string-ends-with-another-string-in-c
    return (
        // Prevent exception of type std::out_of_range: basic_string because the file extension is greater than the filename
        (filename.length() > extension.length()) &&
        // http://stackoverflow.com/questions/874134/find-if-string-ends-with-another-string-in-c
        (0 == filename.compare(filename.length() - extension.length(), extension.length(), extension))
    );
}

void printCount( int count, string description ) {
    if (!bQuiet) {
        std::printf("%c[2K", 27);
        std::cout << "\r  ";
        std::cout.width( 7 );
        std::cout << count << description;
        std::cout << std::flush;
    }
}

void printProgress( int percent, string description ) {
    if (!bQuiet) {
        std::printf("%c[2K", 27);
        std::cout << "\r  ";
        std::cout.width( 6 );
        std::cout << percent << "% " << description;
        std::cout << std::flush;
    }
}

int main(int argc, char** argv) {
    HANDLE hStorage;
    string strSearchPattern = "/";
    string strFilePattern;
    string strFileExt;
    string strSource = "/Applications/Heroes of the Storm";
    string strDestination = ".";
    vector<tSearchResult> searchResults;
    bool bUseFullPath = false;
    bool bLowerCase = false;
    bool bExtract = false;
    bool bPattern = false;
    bool bFileExt = false;
    bool bDirectories = false;
    int filesFound = 0;
    int filesDone = 0;
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

                case OPT_SRC:
                    strSource = args.OptionArg();
                    break;

                case OPT_DEST:
                    strDestination = args.OptionArg();
                    break;

                case OPT_SEARCH:
                    strSearchPattern = args.OptionArg();
                    break;

                case OPT_FILEPTRN:
                    bPattern = true;
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

                case OPT_QUIET:
                    bQuiet = true;
                    break;

                case OPT_VERBOSE:
                    bVerbose = true;
                    break;

                case OPT_EXTRACT:
                    bExtract = true;
                    break;

                case OPT_LISTDIRS:
                    bDirectories = true;
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

    // if (bDirectories) {
        std::set<string> directoryResults;
        std::set<string>::iterator dIter;
        std::pair<std::set<string>::iterator,bool> dRet;
    // }

    // Remove trailing slashes at the end of the storage path (CascLib doesn't like that)
    if ((strSource[strSource.size() - 1] == '/') || (strSource[strSource.size() - 1] == '\\'))
        strSource = strSource.substr(0, strSource.size() - 1);

    // Open CASC Files
    if (!CascOpenStorage(strSource.c_str(), 0, &hStorage)) {
        cerr << "Failed to open the storage '" << strSource << "'" << endl;
        return -2;
    }

    // Explain what we want to do
    echo("Searching for files: \n");
    verbose("* full paths matching '" + strSearchPattern + "'\n");
    if (bPattern) {
        verbose("* filenames matching '" + strFilePattern + "'\n");
    }
    if (bFileExt) {
        verbose("* extensions matching '" + strFileExt + "'\n");
    }
    if (bFileExt || bPattern) {
        verbose();
    }

    // Set up structure
    CASC_FIND_DATA findData;
    HANDLE handle = CascFindFirstFile(hStorage, "*", &findData, NULL);

    if (handle) {

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
                    if ( bDirectories ) {
                        directoryResults.insert(r.strFullPath.substr(0,r.strFullPath.size()-r.strFileName.length()));
                    } else {
                        filesFound++;
                        printCount(filesFound, " matches...");
                        searchResults.push_back(r);
                        verbose(findData.szFileName);
                        verbose();
                    }
                }
            }
        } while (CascFindNextFile(handle, &findData) && findData.szPlainName);

        CascFindClose(handle);
    } else {
        echo("  No files found!\n");
        return -3;
    }
    echo();

    if ( bDirectories ) {
        for (dIter=directoryResults.begin(); dIter!=directoryResults.end(); ++dIter)
            std::cout << ' ' << *dIter << endl;
        std::cout << '\n';
    }

    // Extraction
    if (bExtract && !searchResults.empty())
    {
        char buffer[1000000];

        echo("Extracting files:\n");

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

            int progress = 0;
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
                    cerr << "ERROR: Failed to extract '" << iter->strFullPath << "' to " << strDestName << endl;
                }
                filesDone++;
                progress = (filesDone * 100 / filesFound);
                printProgress(progress, iter->strFullPath);

                CascCloseFile(hFile);
            }
            else
            {
                cerr << "ERROR: Failed to extract '" << iter->strFullPath << "' to " << strDestName << endl;
            }
        }
    }

    CascCloseStorage(hStorage);

    return 0;
}
