--[[--

This script is adpated from link_annotation.c which has the following header.

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

local function print_page(pdf, font, page_num)
  local page = hpdf.AddPage(pdf)
  hpdf.Page_SetWidth(page, 200)
  hpdf.Page_SetHeight(page, 200)
  hpdf.Page_SetFontAndSize(page, font, 20)
  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, 50, 150)
  hpdf.Page_ShowText(page, "Page " .. page_num)
  hpdf.Page_EndText(page)
  return page
end -- print_page

local uri = "http://sourceforge.net/projects/libharu"
local pdf = hpdf.New()
if pdf then

    -- create default-font
    local font = hpdf.GetFont(pdf, "Helvetica")

    -- create index page
    local index_page = hpdf.AddPage(pdf)
    hpdf.Page_SetWidth(index_page, 300)
    hpdf.Page_SetHeight(index_page, 220)

    -- add 7 pages to the document
    local page = {}
    for j = 1, 7 do
      page[j] = print_page(pdf, font, j)
    end -- j

    hpdf.Page_BeginText(index_page)
    hpdf.Page_SetFontAndSize(index_page, font, 10)
    hpdf.Page_MoveTextPos(index_page, 15, 200)
    hpdf.Page_ShowText(index_page, "Link annotation demonstration")
    hpdf.Page_EndText(index_page)

    -- create Link-Annotation object on index page
    hpdf.Page_BeginText(index_page)
    hpdf.Page_SetFontAndSize(index_page, font, 8)
    hpdf.Page_MoveTextPos(index_page, 20, 180)
    hpdf.Page_SetTextLeading(index_page, 23)

    -- page 1 (HPDF_ANNOT_NO_HIGHTLIGHT)
    local tp = hpdf.Page_GetCurrentTextPos(index_page)
    hpdf.Page_ShowText(index_page,
      "Jump to Page 1 (HilightMode=HPDF_ANNOT_NO_HIGHTLIGHT)")
    local rect = {}
    rect.left = tp.x - 4
    rect.bottom = tp.y - 4
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    rect.right = tp.x + 4
    rect.top = tp.y + 10

    hpdf.Page_MoveToNextLine(index_page)
    local dst = hpdf.Page_CreateDestination(page[1])
    local annot = hpdf.Page_CreateLinkAnnot(index_page, rect, dst)
    hpdf.LinkAnnot_SetHighlightMode(annot, "HPDF_ANNOT_NO_HIGHTLIGHT")

    -- page 2 (HPDF_ANNOT_INVERT_BOX)
    tp = hpdf.Page_GetCurrentTextPos(index_page)

    hpdf.Page_ShowText(index_page,
      "Jump to Page 2 (HilightMode=HPDF_ANNOT_INVERT_BOX)")
    rect.left = tp.x - 4
    rect.bottom = tp.y - 4
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    rect.right = tp.x + 4
    rect.top = tp.y + 10

    hpdf.Page_MoveToNextLine(index_page)
    dst = hpdf.Page_CreateDestination(page[2])
    annot = hpdf.Page_CreateLinkAnnot(index_page, rect, dst)
    hpdf.LinkAnnot_SetHighlightMode(annot, "HPDF_ANNOT_INVERT_BOX")

    -- page 3 (HPDF_ANNOT_INVERT_BORDER) */
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    hpdf.Page_ShowText(index_page,
      "Jump to Page 3 (HilightMode=HPDF_ANNOT_INVERT_BORDER)")
    rect.left = tp.x - 4
    rect.bottom = tp.y - 4
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    rect.right = tp.x + 4
    rect.top = tp.y + 10

    hpdf.Page_MoveToNextLine(index_page)
    dst = hpdf.Page_CreateDestination(page[3])
    annot = hpdf.Page_CreateLinkAnnot(index_page, rect, dst)
    hpdf.LinkAnnot_SetHighlightMode(annot, "HPDF_ANNOT_INVERT_BORDER")

    -- page 4 (HPDF_ANNOT_DOWN_APPEARANCE) */
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    hpdf.Page_ShowText(index_page,
      "Jump to Page 4 (HilightMode=HPDF_ANNOT_DOWN_APPEARANCE)")
    rect.left = tp.x - 4
    rect.bottom = tp.y - 4
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    rect.right = tp.x + 4
    rect.top = tp.y + 10

    hpdf.Page_MoveToNextLine(index_page)
    dst = hpdf.Page_CreateDestination(page[4])
    annot = hpdf.Page_CreateLinkAnnot(index_page, rect, dst)
    hpdf.LinkAnnot_SetHighlightMode(annot, "HPDF_ANNOT_DOWN_APPEARANCE")

    -- page 5 (dash border)
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    hpdf.Page_ShowText(index_page, "Jump to Page 5 (dash border)")
    rect.left = tp.x - 4
    rect.bottom = tp.y - 4
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    rect.right = tp.x + 4
    rect.top = tp.y + 10

    hpdf.Page_MoveToNextLine(index_page)
    dst = hpdf.Page_CreateDestination(page[5])
    annot = hpdf.Page_CreateLinkAnnot(index_page, rect, dst)
    hpdf.LinkAnnot_SetBorderStyle(annot, 1, 3, 2)

    -- page 6 (no border)
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    hpdf.Page_ShowText(index_page, "Jump to Page 6 (no border)")
    rect.left = tp.x - 4
    rect.bottom = tp.y - 4
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    rect.right = tp.x + 4
    rect.top = tp.y + 10

    hpdf.Page_MoveToNextLine(index_page)
    dst = hpdf.Page_CreateDestination(page[6])
    annot = hpdf.Page_CreateLinkAnnot(index_page, rect, dst)
    hpdf.LinkAnnot_SetBorderStyle(annot, 0, 0, 0)

    -- page 7 (bold border)
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    hpdf.Page_ShowText(index_page, "Jump to Page 7 (bold border)")
    rect.left = tp.x - 4
    rect.bottom = tp.y - 4
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    rect.right = tp.x + 4
    rect.top = tp.y + 10

    hpdf.Page_MoveToNextLine(index_page)
    dst = hpdf.Page_CreateDestination(page[7])
    annot = hpdf.Page_CreateLinkAnnot(index_page, rect, dst)
    hpdf.LinkAnnot_SetBorderStyle(annot, 2, 0, 0)

    -- URI link
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    hpdf.Page_ShowText(index_page, "URI (")
    hpdf.Page_ShowText(index_page, uri)
    hpdf.Page_ShowText(index_page, ")")

    rect.left = tp.x - 4
    rect.bottom = tp.y - 4
    tp = hpdf.Page_GetCurrentTextPos(index_page)
    rect.right = tp.x + 4
    rect.top = tp.y + 10

    hpdf.Page_CreateURILinkAnnot(index_page, rect, uri)
    hpdf.Page_EndText(index_page)

    -- save the document to a file
    save_to_file(pdf, arg[0])

    -- clean up
    hpdf.Free(pdf)

else
  io.write("Error creating PDF object\n")
end -- if
