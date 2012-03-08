Demonstration scripts 
=====================

The demo directory contains some of the demonstration programs included in the
Haru library distribution. These have been closely translated from C to Lua.
Each script generates a PDF file with the same name as the script except for
the extension which is changed from `.lua` to `.pdf`.

On Unix-type systems, execute the following command to generate the PDF files:

    make demo

This will build the LuaHPDF binding if necessary, and then invoke Lua to
generate each demonstration file. The `package.path` and `package.cpath`
variables are modified on the command line so that demonstration scripts load
hpdf.so and common.lua as modules.

The common.lua script is a helper module that you may wish to install in a
standard Lua module directory. Run

    lua -e "print(package.path)"

to see where these are.
