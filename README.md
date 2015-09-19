Coco/R
------

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

- Fix for MinGW compile.
- Fix EOL handling.
- Add Taste mini-compiler from original source.
- Include calling coco.exe in the build process.
- Add Visual Studio solution files.
- Write error messages to stderr, exit false if there were any errors.