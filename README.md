![LuaHPDF](doc/luahpdf.png?raw=true "LuaHPDF")

LuaHPDF is a module that lets you programmatically create PDF files using
[Lua][1]. It does this by binding to the [Haru Free PDF Library][2], an
open-source library that manages the details of PDF generation.

With this module, you can write Lua programs that will produce PDF files that
include

* lines, text and graphics
* PNG and JPEG images
* compression
* encryption
* Type1 and TrueType fonts
* various character sets
* outlines
* links

This module works on POSIX and Windows platforms.

Implementation
==============

LuaHPDF conforms to the Lua 5.2 module standard. It provides a [thin
binding][3] to the libharu library for PDF generation. The [differences][7] to
the Haru application programming interface are minor, so you should be able to
write Lua scripts using the documentation that is provided with the Haru
library.

Building and installing LuaHPDF
===============================

The Haru library and Lua must be properly installed on your system to build
LuaHPDF. Each can be compiled easily from source code. Alternatively, each is
available in binary form for a wide variety of platforms. See their respective
websites for more details.

Notes on [building LuaHPDF][6] guide you through compiling and installing the
module on your system. Check out some of the features of the Haru library by
[generating demonstration PDFs][12].

Creating PDF files
==================

The steps in creating a basic PDF file with LuaHPDF include

* loading the module with the 'require' function
* creating a document
* adding one or more pages
* obtaining a font and registering it with a page
* writing text
* saving the file
* freeing the document

Here is a short example.

    require "hpdf"

    local pdf = hpdf.New()
    if pdf then
      local page = hpdf.AddPage(pdf)
      local height = hpdf.Page_GetHeight(page)
      local width = hpdf.Page_GetWidth(page)
      local font = hpdf.GetFont(pdf, "Helvetica")
      hpdf.Page_SetFontAndSize(page, font, 24)
      hpdf.Page_BeginText(page)
      hpdf.Page_TextOut(page, 60, height - 60, "Hello from Haru")
      hpdf.Page_EndText(page)
      hpdf.SaveToFile(pdf, "hello.pdf")
      hpdf.Free(pdf)
    else
      io.write("Error creating PDF object\n")
    end

Other demonstration scripts are provided in the download package.

Reporting issues
================

You're encouraged to provide suggestions and bug reports about LuaHPDF
in this software's [issue tracker][11].

Thanks
======

LuaHPDF depends on other software. In particular, I am grateful to

* The authors of [Lua][1], Roberto Ierusalimschy, Luiz Henrique de Figueiredo,
  Waldemar Celes.

* The orignal author of the [Haru Free PDF Library][2], Takeshi Kanno.

* Andrea D'Amore who provided tips for compiling on OS X and packaging.

* Clerk Ma who provided instructions and makefiles for building with Visual Studio.

License
=======

Like Lua, this software is released under the [MIT license][5].

[1]:  http://www.lua.org/
[2]:  http://libharu.org/
[3]:  doc/binding-notes.md
[5]:  doc/license.md
[6]:  doc/build.md
[7]:  doc/binding-changes.md
[8]:  doc/change-log.md
[11]: https://github.com/jung-kurt/luahpdf/issues
[12]: doc/demos.md

