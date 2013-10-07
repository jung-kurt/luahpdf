--[[--

This script is adpated from outline_demo.c which has the following header.

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

local function print_page(page, page_num)
  hpdf.Page_SetWidth(page, 800)
  hpdf.Page_SetHeight(page, 800)
  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, 30, 740)
  hpdf.Page_ShowText(page, "Page " .. page_num)
  hpdf.Page_EndText(page)
end -- print_page

local function create_dst(page, outline)
  local dst = hpdf.Page_CreateDestination(page)
  hpdf.Destination_SetXYZ(dst, 0, hpdf.Page_GetHeight(page), 1)
  hpdf.Outline_SetDestination(outline, dst)
end -- create_dst

local pdf = hpdf.New()
if pdf then

  -- create default-font
  local font = hpdf.GetFont(pdf, "Helvetica")

  -- set page mode to use outlines
  hpdf.SetPageMode(pdf, "HPDF_PAGE_MODE_USE_OUTLINE")

  -- add 3 pages to the document
  local page = {}
  for j = 1, 3 do
    page[j] = hpdf.AddPage(pdf)
    hpdf.Page_SetFontAndSize(page[j], font, 30)
    print_page(page[j], j)
  end -- j

  -- create outline root
  local root = hpdf.CreateOutline(pdf, nil, "OutlineRoot")
  hpdf.Outline_SetOpened(root, true)

  local outline = {}
  outline[1] = hpdf.CreateOutline(pdf, root, "Page 1")
  outline[2] = hpdf.CreateOutline(pdf, root, "Page 2")

  -- create outline with test which is ISO8859-2 encoding
  outline[3] = hpdf.CreateOutline(pdf, root, "ISO8859-2 text гдежзий",
    hpdf.GetEncoder(pdf, "ISO8859-2"))

  -- create destination objects on each pages and link it to outline items
  for j = 1, 3 do
    create_dst(page[j], outline[j])
  end -- j

  -- save the document to a file
  save_to_file(pdf, arg[0])

  -- clean up
  hpdf.Free(pdf)

else
  io.write("Error creating PDF object\n")
end -- if
