--[[--

This script is adpated from encryption.c which has the following header.

Haru Free PDF Library 2.0.0

Copyright (c) 1999-2006 Takeshi Kanno <takeshi_kanno@est.hi-ho.ne.jp>

Permission to use, copy, modify, distribute and sell this software and its
documentation for any purpose is hereby granted without fee, provided that the
above copyright notice appear in all copies and that both that copyright notice
and this permission notice appear in supporting documentation.

It is provided "as is" without express or implied warranty.

--]]--

local hpdf = require "hpdf"
require "common"

local pdf = hpdf.New()
if pdf then
  local text =
    "Eat this encrypted document after you have committed it to memory."
  local page = hpdf.AddPage(pdf)
  local font = hpdf.GetFont(pdf, "Times-Roman")
  hpdf.Page_SetSize(page, "HPDF_PAGE_SIZE_B5", "HPDF_PAGE_LANDSCAPE")
  hpdf.Page_BeginText(page)
  hpdf.Page_SetFontAndSize(page, font, 14)
  local tw = hpdf.Page_TextWidth(page, text)
  hpdf.Page_MoveTextPos(page, (hpdf.Page_GetWidth(page) - tw) / 2,
    (hpdf.Page_GetHeight(page) - 20) / 2)
  hpdf.Page_ShowText(page, text)
  hpdf.Page_EndText (page)
  hpdf.SetPassword(pdf, "owner", "user")
  save_to_file(pdf, arg[0])
  hpdf.Free(pdf)
else
  io.write("Error creating PDF object\n")
end -- if
