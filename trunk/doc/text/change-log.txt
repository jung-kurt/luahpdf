Change log
==========

This is a list of changes to LuaHPDF. The current version is 1.3.

Version 1.5: 2013-11-07
-----------------------

* Adjustments to Makefile for Mac OS X

Version 1.4: 2013-10-07
-----------------------

Lua 5.2 compatibility update

* Adjustments to Makefile

* Assign module table from call to require() in demonstration scripts

* Replace outmoded C environment with table shared via upvalues.

Version 1.3: 2012-12-21
-----------------------

* Instructions for building LuaHPDF with Visual Studio

* Makefile adjustments to build with [libharu.org][3] source as Antony Dovgal's
  fork is now evidently even less supported than the libharu version

Version 1.2: 2012-03-08
-----------------------

* Guard against passing name of unwriteable file to SaveToFile; thanks and.damore

* Replace grid_sheet.lua in parent directory with common.lua in demo directory

* Return number of characters printed by hpdf.Page_TextRect()

Version 1.1: 2012-02-23
-----------------------

* Corrected bug with Font_TextWidth result table; thanks mjwhite0 for spotting this
  and providing fix
  
* Define HPDF_HANDLE for static builds  

Version 1.0: 2010-11-10
-----------------------

* Improved support for OS X

* Link with Antony Dovgal's version of libharu (*inaccessible as of Dec 2012*)

* Guarded against assignment of zero element in pattern array passed to
  [HPDF\_Page\_SetDash()][2]

* Small documentation updates

Version 0.1: 2007-02-16
-----------------------

* Initial release

To do
=====

Here are wishlist items for the LuaHPDF binding.

* Test suite

* More demonstration scripts

* A higher-level interface to support text wrapping and styling

[2]: http://libharu.org/wiki/Documentation/API/Graphics#HPDF_Page_SetDash.28.29
[3]: http://libharu.org/
