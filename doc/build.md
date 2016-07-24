Building LuaHPDF
================

Both Lua and the Haru Free PDF library must be installed on your system in
order to build LuaHPDF. This assures that the C compiler and linker will be
able to locate the include files and libraries of both Lua and the Haru
library.

Building on Unix-type systems
-----------------------------

These instructions assume that the shared library version of the Haru Free PDF
library has been installed on your system. To do this, specify `./configure
--shared` when compiling it.

Examine the top portion of the included Makefile and, if needed, edit it to
suit your environment. In particular, uncomment the appropriate PLATFORM
(currently, linux or macosx) and comment the other.

`PREFIX` specifies the directory that contains the lib/lua/5.2 path into which
the HPDF binding will be installed. By default, this is `/usr/local`. If your
standard directory for Lua modules is different than `/usr/local/lib/lua/5.2`,
then you will likely want to change this. For example, some distributions set
up `/usr/lib/lua/5.2` as the Lua module directory; for these, you should set
`PREFIX` to `/usr`.

`MODDIR` specifies the path under `PREFIX` into which the compiled binding will
be placed.

`DOCDIR` specifies the path under `PREFIX` into which the HTML documentation
will be installed.

`LUALIB` by default is set to `-llua`. Your environment may require you to
replace this with something like `-llua5.2`.

`LUAINC` by default is empty, meaning the Lua include files can be found in one
of the standard places. Your environment may require you to replace this with
something like `-I/usr/include/lua5.2`.

Similarly, `HPDFLIB` and `HPDFINC` are by default `-lhpdf` and empty. Change
these to suit your environment if needed.

To compile luahpdf.c into a library, from the command prompt execute

    make

To make the documentation, execute

    make doc

To create sample PDFs using the binding, run the demonstration scripts as
follows:

    make demo

To install the binding and its documentation, execute

    sudo make install

or, to specify an alternative destination, execute something like

    sudo make DESTDIR=/tmp install

The `sudo` command may be omitted if you have permission to write to the
directory specified by `DESTDIR`.

Building a Windows binding with MinGW
-------------------------------------

A makefile that works with the MinGW tool chain is provided. It has been tested
with the [MinGW Cross Compiler][2] on Linux, but should work with the MinGW
tools hosted on Windows using the MSYS shell.

Prior to building the binding, you'll need to make sure the Lua and HPDF DLLs
and header files have been installed in a place where the compiler can find
them. You may need to edit the `MINGW` variable in Makefile.mingw to specify
the location on your system.

To compile luahpdf.c into a dynamic link library, from the command prompt
execute

    make -f Makefile.mingw

To make the documentation, execute

    make -f Makefile.mingw doc

You'll need to manually copy hpdf.dll to the appropriate directory (part of
LUA_CPATH) on your Windows system.

Building a Windows binding with Visual Studio
---------------------------------------------

Clerk Ma has provided a README file and makefiles in the `windows` directory
for Visual Studio users.

Testing the installation
------------------------

To verify that the library can be properly loaded by Lua, execute the following
command:

    lua -l hpdf -e "print(hpdf.VERSION_TEXT)"

The Haru library version should be printed. If Lua reports that the hpdf module
cannot be found, examine the subsequent lines to see where Lua actually looked
for the module.

See the [demos][1] page for instructions on generating the demonstration PDF
files.

[1]: demos.md
[2]: http://mingw-cross.sourceforge.net/
