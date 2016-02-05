# storm-extract

A command-line tool to list and extract files from the Heroes of the Storm CASC
(Content Addressable Storage Container) archives.

Works on MacOS X and Linux.  I have neither the time nor the resources to build
compile and test on Windows.


## Dependencies

The following libraries are necessary to build **storm-extract**:

* CascLib (http://www.zezula.net/en/casc/main.html), MIT license
* SimpleOpt 3.4 (http://code.jellycan.com/simpleopt/), MIT License (included)

To download the CascLib submodule, do:

    ~$ cd <path/to/the/source/of/storm-extract>
    storm-extract$ git submodule init
    storm-extract$ git submodule update

## Building

Requires <a href="http://www.cmake.org/">cmake</a> to build:

    $ mkdir build
    $ cd build
    $ cmake <path/to/the/source/of/storm-extract>
    $ make

The executable will be put in build/bin/

## Usage

```
storm-extract
Usage: bin/storm-extract [options] <CASC_ROOT> <PATTERN>

This program can list and optionally extract files from a CASC storage container.

Options:
    -h, --help                Display this help
    -v, --verbose             Prints actions taken
    -p, --pattern <STRING>    Search for filenames matching STRING
    -e, --extension <STRING>  Search for filenames having extension STRING
    -x, --extract             Extract the files found
    -o, --dest <PATH>         The folder where the files are extracted (extract only)
                                (default: current working directory)
    -f, --fullpath            During extraction, preserve the path hierarchy found
                                inside the storage (extract only)
    -c, --lowercase           Convert extracted file paths to lowercase (extract only)

Examples:

  1) List all files in CASC storage container:

       ./storm-extract "/Applications/Heroes of the Storm/" /

  2) Extract a specific file:

       ./storm-extract -o out "/Applications/Heroes of the Storm/" "Path/To/The/File"

```

## Credits

**storm-extract** stands on the shoulders of giants.


The library absolutely, unequivocably, could not be possible without
[ladislav-zezula's CascLib](https://github.com/ladislav-zezula/CascLib)
library.  Many thanks to [ladislav-zezula](https://github.com/ladislav-zezula).

Most of the program was canibalized from
[Kanma's CASCExtractor](https://github.com/Kanma/CASCExtractor/) with the purpose
of customizing it for Heroes of the Storm and integration into
[NodeJS](https://www.nodejs.org).


## License

**storm-extract** is made available under the MIT License. The text of the license is in
the file `LICENSE`.

Under the MIT License you may use **storm-extract** for any purpose you wish, without warranty,
and modify it if you require, subject to one condition:

>   "The above copyright notice and this permission notice shall be included in
>   all copies or substantial portions of the Software."

In practice this means that whenever you distribute your application, whether as binary or
as source code, you must include somewhere in your distribution the text in the file
'LICENSE'. This might be in the printed documentation, as a file on delivered media, or
even on the credits / acknowledgements of the runtime application itself; any of those
would satisfy the requirement.

Even if the license doesn't require it, please consider to contribute your modifications
back to the community.
