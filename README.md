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
storm-extract v0.1.1
  Usage: storm-extract [options]

This program can list and optionally extract files from a Heroes of the Storm CASC storage container.

    -h, --help                Display this help

Options:
  Common:
    -i, --in <PATH>           Directory where '/HeroesData' is
                                (default: '/Applications/Heroes of the Storm')
    -v, --verbose             Prints more information
    -q, --quiet               Prints nothing, nada, zip
    -s, --search <STRING>     Restrict results to full paths matching STRING
    -f, --filename <STRING>   Search for filenames matching STRING
    -e, --extension <STRING>  Search for filenames having extension STRING

  Search:     storm-extract [options]

  Extract:    storm-extract -x [options]
    -x, --extract             Extract the files found
    -o, --out <PATH>          The folder where the files are extracted (extract only)
                                (default: current working directory)
    -p, --path                During extraction, preserve the path hierarchy found
                                inside the storage (extract only)
    -c, --lowercase           Convert extracted file paths to lowercase (extract only)

  Directory:  storm-extract -d [options]
    -d, --directories         Print all directories found

Examples:

  1) List all files in CASC storage container (this will take a while):

       ./storm-extract -i "/Applications/Heroes of the Storm/" -f /

  2) Extract a specific file:

       ./storm-extract -i "/Applications/Heroes of the Storm/" -f "path/to/the/file" -o out

  3) Extract specific filenames:

       ./storm-extract -i "/Applications/Heroes of the Storm/" -f GameData.xml -o out -p -x

  4) Extract all English sounds:

       ./storm-extract -i "/Applications/Heroes of the Storm/" -s enus -o out -e wav -p -x
       ./storm-extract -i "/Applications/Heroes of the Storm/" -s enus -o out -e ogg -p -x

Copyright(c) 2016 Justin J. Novack
https://www.github.com/jnovack/storm-extract
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
