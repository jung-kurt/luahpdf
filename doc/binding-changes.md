Differences from the Haru API
=============================

LuaHPDF is a relatively transparent binding to the Haru free PDF library, so it
should be straightforward to write Lua scripts using the Haru documentation.
The differences that you will encounter are summarized here.

Names of functions and constants
--------------------------------

The names of functions and constants in the Haru library begin with the prefix
`HPDF_`. In LuaHPDF, this prefix has been removed and the remainder of the name
is used as a key in the `hpdf` namespace. For example, `HPDF_GetFont` is named
`hpdf.GetFont` in LuaHPDF. The integer, float and string manifest constants
defined in hpdf_consts.h are treated similarly. For example, the constant
defined by the line

    #define HPDF_ENABLE_PRINT        4

is available as `hpdf.ENABLE_PRINT`.

Buffers and callbacks
---------------------

Currently, bindings to the `HPDF_NewEx`, `HPDF_SetErrorHandler` and
`HPDF_ReadFromStream` functions have not been implemented because of memory
buffer and callback issues.

Enumerations
------------

In hpdf_types.h, the Haru library defines several enumerations such as the
following.

    typedef enum _HPDF_PageMode {
      HPDF_PAGE_MODE_USE_NONE = 0,
      HPDF_PAGE_MODE_USE_OUTLINE,
      HPDF_PAGE_MODE_USE_THUMBS,
      HPDF_PAGE_MODE_FULL_SCREEN,
      HPDF_PAGE_MODE_EOF
    } HPDF_PageMode;

In LuaHPDF, a function expecting an argument of type `HPDF_PageMode` should be
passed a string whose value is the same as the enumeration identifier with the
leading `HPDF_` removed. That is, the argument should be the string
`"PAGE_MODE_USE_NONE"`, `"PAGE_MODE_USE_OUTLINE"`, `"PAGE_MODE_USE_THUMBS"`, or
`"PAGE_MODE_FULL_SCREEN"`. An error occurs if the passed-in argument isn't a
string with one of these values.

This approach is used for all Haru library enumerations.

Structures
----------

Structures in the Haru library are implemented as tables in LuaHPDF. For
example, the CMYK color structure is defined in hpdf_types.h as follows.

    typedef struct _HPDF_CMYKColor {
      HPDF_REAL   c;
      HPDF_REAL   m;
      HPDF_REAL   y;
      HPDF_REAL   k;
    } HPDF_CMYKColor;

In LuaHPDF, this structure is mapped to a Lua table containing the fields `c`,
`m`, `y`, and `k`.

Modified function interfaces
----------------------------

The following changes in function arguments and return values have been made in
LuaHPDF.

* *document* = `hpdf.New()`

    This function returns a document handle. Unlike `HPDF_New` in the Haru API,
    it does not take any arguments.

* *width*, *height* = `hpdf.Image_GetSize2(`*image*`)`

    Unlike `HPDF_Image_GetSize2` in the Haru API, this function accepts only
    the image handle and returns the width and height of the image.

* *x*, *y* = `hpdf.Page_GetCurrentPos2(`*page*`)`

    Unlike `HPDF_Page_GetCurrentPos2` in the Haru API, this function accepts
    only the page handle and returns the current x and y path painting
    positions.

* *x*, *y* = `hpdf.Page_GetCurrentTextPos2(`*page*`)`

    Unlike `HPDF_Page_GetCurrentTextPos2` in the Haru API, this function
    accepts only the page handle and returns the current x and y text rendering
    positions.
    
* *result*, *len* = `hpdf.Page_TextRect(`*page*`,` *left*`,` *top*`,` *right*`,` *bottom*`,` *text*`,` *align*`)`

    Unlike HPDF_Page_TextRect` in the Haru API, this function returns the 
    function return code followed by the number of characters printed in the 
    area.

Various functions accept arguments and return values of type `HPDF_BOOL`. In
these cases, a Lua boolean is used. Note that the value 0, since it is neither
nil nor false, is interpreted as true. The manifest constants `hpdf.TRUE` and
`hpdf.FALSE` are defined as the Lua booleans true and false, respectively.
