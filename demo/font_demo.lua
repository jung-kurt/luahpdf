--[[--

This script is adpated from font_demo.c which has the following header.

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

local font_list = {
  "Courier",
  "Courier-Bold",
  "Courier-Oblique",
  "Courier-BoldOblique",
  "Helvetica",
  "Helvetica-Bold",
  "Helvetica-Oblique",
  "Helvetica-BoldOblique",
  "Times-Roman",
  "Times-Bold",
  "Times-Italic",
  "Times-BoldItalic",
  "Symbol",
  "ZapfDingbats"}

local page_title = "Font demonstration"

local pdf = hpdf.New()
if pdf then

  -- Add a new page object.
  local page = hpdf.AddPage(pdf)
  local height = hpdf.Page_GetHeight(page)
  local width = hpdf.Page_GetWidth(page)

  -- Print the lines of the page.
  hpdf.Page_SetLineWidth(page, 1)
  hpdf.Page_Rectangle(page, 50, 50, width - 100, height - 110)
  hpdf.Page_Stroke(page)

  -- Print the lines of the page. */
  hpdf.Page_SetLineWidth(page, 1)
  hpdf.Page_Rectangle(page, 50, 50, width - 100, height - 110)
  hpdf.Page_Stroke(page)

  -- Print the title of the page (with positioning center).
  local def_font = hpdf.GetFont(pdf, "Helvetica")
  hpdf.Page_SetFontAndSize(page, def_font, 24)

  local tw = hpdf.Page_TextWidth(page, page_title)
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, (width - tw) / 2, height - 50, page_title)
  hpdf.Page_EndText(page)

  -- Output subtitle.
  hpdf.Page_BeginText(page);
  hpdf.Page_SetFontAndSize(page, def_font, 16)
  hpdf.Page_TextOut(page, 60, height - 80, "Standard Type1 fonts samples")
  hpdf.Page_EndText(page)

  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, 60, height - 105)

  for j, font_str in ipairs(font_list) do

    local samp_text = "abcdefgABCDEFG12345!#$%&+-@?"
    local font = hpdf.GetFont(pdf, font_str)

    -- print a label of text
    hpdf.Page_SetFontAndSize(page, def_font, 9)
    hpdf.Page_ShowText(page, font_str)
    hpdf.Page_MoveTextPos(page, 0, -18)

    -- print a sample text.
    hpdf.Page_SetFontAndSize(page, font, 20)
    hpdf.Page_ShowText(page, samp_text)
    hpdf.Page_MoveTextPos(page, 0, -20)

  end -- j, font_str

  hpdf.Page_EndText(page)
  save_to_file(pdf, arg[0])
  hpdf.Free(pdf)

else
  io.write("Error creating PDF object\n")
end -- if
