--[[--

This script is adpated from slideshow_demo.c which has the following header.

Haru Free PDF Library 2.0.0

Copyright(c) 1999-2006 Takeshi Kanno <takeshi_kanno@est.hi-ho.ne.jp>

Permission to use, copy, modify, distribute and sell this software and its
documentation for any purpose is hereby granted without fee, provided that the
above copyright notice appear in all copies and that both that copyright notice
and this permission notice appear in supporting documentation.

It is provided "as is" without express or implied warranty.

--]]--

local hpdf = require "hpdf"
require "common"

local function print_page(page, style, font, page_prev, page_next)

    local r = math.random()
    local g = math.random()
    local b = math.random()

    hpdf.Page_SetWidth(page, 800)
    hpdf.Page_SetHeight(page, 600)

    hpdf.Page_SetRGBFill(page, r, g, b)

    hpdf.Page_Rectangle(page, 0, 0, 800, 600)
    hpdf.Page_Fill(page)

    hpdf.Page_SetRGBFill(page, 1 - r, 1 - g, 1 - b)

    hpdf.Page_SetFontAndSize(page, font, 30)

    hpdf.Page_BeginText(page)
    hpdf.Page_SetTextMatrix(page, 0.8, 0.0, 0.0, 1.0, 0.0, 0.0)
    hpdf.Page_TextOut(page, 50, 530, style)

    hpdf.Page_SetTextMatrix(page, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0)
    hpdf.Page_SetFontAndSize(page, font, 20)
    hpdf.Page_TextOut(page, 55, 300,
      'Type "Ctrl+L" in order to return from full screen mode.')
    hpdf.Page_EndText(page)

    hpdf.Page_SetSlideShow(page, style, 5.0, 1.0)

    hpdf.Page_SetFontAndSize(page, font, 20)

    local function link(Lf, Rt, Str, PageRef)
      hpdf.Page_BeginText(page)
      hpdf.Page_TextOut(page, Lf, 50, Str)
      hpdf.Page_EndText(page)

      local rect = {left = Lf, right = Rt, top = 70, bottom = 50}
      local dst = hpdf.Page_CreateDestination(PageRef)
      hpdf.Destination_SetFit(dst)
      local annot = hpdf.Page_CreateLinkAnnot(page, rect, dst)
      hpdf.LinkAnnot_SetBorderStyle(annot, 0, 0, 0)
      hpdf.LinkAnnot_SetHighlightMode(annot, "HPDF_ANNOT_INVERT_BOX")
    end -- link

    if page_next then
      link(680, 750, "Next =>", page_next)
    end -- if

    if page_prev then
      link(50, 110, "<= Prev", page_prev)
    end -- if

end -- print_page

local pdf = hpdf.New()
if pdf then

    -- create default-font
    local font = hpdf.GetFont(pdf, "Courier")

    local StyleList = {
      "HPDF_TS_WIPE_RIGHT",
      "HPDF_TS_WIPE_UP",
      "HPDF_TS_WIPE_LEFT",
      "HPDF_TS_WIPE_DOWN",
      "HPDF_TS_BARN_DOORS_HORIZONTAL_OUT",
      "HPDF_TS_BARN_DOORS_HORIZONTAL_IN",
      "HPDF_TS_BARN_DOORS_VERTICAL_OUT",
      "HPDF_TS_BARN_DOORS_VERTICAL_IN",
      "HPDF_TS_BOX_OUT",
      "HPDF_TS_BOX_IN",
      "HPDF_TS_BLINDS_HORIZONTAL",
      "HPDF_TS_BLINDS_VERTICAL",
      "HPDF_TS_DISSOLVE",
      "HPDF_TS_GLITTER_RIGHT",
      "HPDF_TS_GLITTER_DOWN",
      "HPDF_TS_GLITTER_TOP_LEFT_TO_BOTTOM_RIGHT",
      "HPDF_TS_REPLACE"}

    local page = {}

    for J in ipairs(StyleList) do
      page[#page + 1] = hpdf.AddPage(pdf)
    end -- J

    for J, Style in ipairs(StyleList) do
      -- Don't worry about out-of-bound references; print_page is expecting them
      print_page(page[J], Style, font, page[J - 1], page[J + 1])
    end -- J, Style

    hpdf.SetPageMode(pdf, "HPDF_PAGE_MODE_FULL_SCREEN")

    -- save the document to a file
    save_to_file(pdf, arg[0])

    -- clean up
    hpdf.Free(pdf)

else
  io.write("Error creating PDF object\n")
end -- if
