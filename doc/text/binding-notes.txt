Notes about the Lua binding
===========================

Lua provides a perfect mechanism for managing typesafe handles that are held
exclusively by a Lua script to access a library programming interface. The
functions in the HPDF library are handle-based, but the handles created by the
library are held both by the script and the library. For example, the function
`HPDF_Page_GetCurrentFont` will return the font handle that is currently
registered in a page, even though the userdata that binds to this handle may
have fallen out of scope in the controlling Lua script. Furthermore, handles
can survive the document with which they were created and be reused in another
document.

Light userdata
--------------

To address these aspects of the HPDF library, the Lua binding can simply use
light userdata to hold the value of handles. In this case, type-safety can be
achieved by means of a table in the C environment that maps the light userdata
to integers or strings that identify their handle type. In this case, the type
of all userdata that are passed to the binding are verified before passing the
corresponding handle to the HPDF library. Every time a handle is created from
the HPDF library, it is associated with its type in this table.

Full userdata
-------------

Alternatively, full userdata can be used to represent HPDF handles. This
approach allows the userdata to be associated with a metatable. The benefits of
this include the type-safety the metatable provides and the use of the `__index`
metamethod to support methods bound to the userdata. This simplifies the Lua
script by replacing a call like

    hpdf.GetFont(Doc, "Helvetica")

to

    Doc:GetFont("Helvetica")

The downside is that the userdata need to be preserved even after they fall out
of scope in the Lua script, because functions like `HPDF_Page_GetCurrentFont`
will need to return them. An ordinary table (not weak) in the C environment can
be used to map HPDF handles to full userdata, thus keeping the userdata from
being collected.

Garbage collection
------------------

The problem with both solutions is that a table in the C environment may
accumulate userdata (light in the first case and full in the second) that no
longer correspond to active handles. This is simply a garbage collection issue;
since the userdata stored in the environment can't be accessed from the Lua
script, they don't pose a programmatic risk other than consuming memory. Even
the memory waste is negligible in most cases; however, in the case where this
binding is used in long-running applications it could become an issue.
Consequently, an additional function that clears out this table is warranted.
It can be called by a long-running script at suitable points where all handles
are known to be closed. This could possibly be done when the function
`HPDF_FreeDocAll` is called and the specified document is the only one active.

LuaHPDF implementation
----------------------

The bulk of the LuaHPDF source file hpdf.c was automatically generated from the
main header file of the Haru library, hpdf.h, using a Lua script. It uses light
userdata to represent handles in the Haru library.
