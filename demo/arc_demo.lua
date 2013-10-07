--[[--

This script is adpated from arc_demo.c which has the following header.

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

local pdf = hpdf.New()
if pdf then
  -- add a new page object
  local page = hpdf.AddPage(pdf)

  hpdf.Page_SetHeight(page, 220)
  hpdf.Page_SetWidth(page, 200)

  -- draw grid to the page
  hpdf.print_grid(pdf, page)

  -- draw pie chart
  --
  --   A: 45% Red
  --   B: 25% Blue
  --   C: 15% green
  --   D: other yellow

  -- A
  hpdf.Page_SetRGBFill(page, 1.0, 0, 0)
  hpdf.Page_MoveTo(page, 100, 100)
  hpdf.Page_LineTo(page, 100, 180)
  hpdf.Page_Arc(page, 100, 100, 80, 0, 360 * 0.45)
  local x, y = hpdf.Page_GetCurrentPos2(page)
  hpdf.Page_LineTo(page, 100, 100)
  hpdf.Page_Fill(page)

  -- B
  hpdf.Page_SetRGBFill(page, 0, 0, 1.0)
  hpdf.Page_MoveTo(page, 100, 100)
  hpdf.Page_LineTo(page, x, y)
  hpdf.Page_Arc(page, 100, 100, 80, 360 * 0.45, 360 * 0.7)
  x, y = hpdf.Page_GetCurrentPos2(page)
  hpdf.Page_LineTo(page, 100, 100)
  hpdf.Page_Fill(page)

  -- C
  hpdf.Page_SetRGBFill(page, 0, 1.0, 0)
  hpdf.Page_MoveTo(page, 100, 100)
  hpdf.Page_LineTo(page, x, y)
  hpdf.Page_Arc(page, 100, 100, 80, 360 * 0.7, 360 * 0.85)
  x, y = hpdf.Page_GetCurrentPos2(page)
  hpdf.Page_LineTo(page, 100, 100)
  hpdf.Page_Fill(page)

  -- D
  hpdf.Page_SetRGBFill(page, 1.0, 1.0, 0)
  hpdf.Page_MoveTo(page, 100, 100)
  hpdf.Page_LineTo(page, x, y)
  hpdf.Page_Arc(page, 100, 100, 80, 360 * 0.85, 360)
  x, y = hpdf.Page_GetCurrentPos2(page)
  hpdf.Page_LineTo(page, 100, 100)
  hpdf.Page_Fill(page)

  -- draw center circle
  hpdf.Page_SetGrayStroke(page, 0)
  hpdf.Page_SetGrayFill(page, 1)
  hpdf.Page_Circle(page, 100, 100, 30)
  hpdf.Page_Fill(page)

  -- save the document to a file
  save_to_file(pdf, arg[0])

  -- clean up
  hpdf.Free(pdf)

  return 0
else
  io.write("Error creating PDF object\n")
end -- if
