Coco/R ANSI
-----------

This is a compiler generator for C++. To build the binary, run

    make

The executable will be in this directory. Use it as you want.

Documentation is available at the original site for Coco/R.

[The Compiler Generator Coco/R website](http://www.ssw.uni-linz.ac.at/Coco/)

Changes to original Coco/R for C++
----------------------------------

This is the list of main changes. Please check the 
[GIT repository](https://github.com/pauloscustodio/coco-r-cpp) 
for details.

- Change to parse only ANSI (8-bit) input files - no need for conversion 
  between UTF-8 and wchar_t multiple times depending on current need.
  Target of the use of this Coco/R ANSI is parsing of computer languages
  that do not need Unicode input.
- Write error messages to stderr, exit false if there were any errors.
- Fix EOL handling for three different architectures - Unix, Win32 and old Mac.
- Fix for MinGW compile.
- Add Taste mini-compiler from original source and use it in test scripts.
- Include calling coco.exe in the build process.
- Add Visual Studio solution files.
