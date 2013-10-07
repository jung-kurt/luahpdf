--[[--

This script is adpated from ext_gstate_demo.c which has the following header.

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

local function draw_circles(page, description, x, y)
  hpdf.Page_SetLineWidth(page, 1.0)
  hpdf.Page_SetRGBStroke(page, 0.0, 0.0, 0.0)
  hpdf.Page_SetRGBFill(page, 1.0, 0.0, 0.0)
  hpdf.Page_Circle(page, x + 40, y + 40, 40)
  hpdf.Page_ClosePathFillStroke(page)
  hpdf.Page_SetRGBFill(page, 0.0, 1.0, 0.0)
  hpdf.Page_Circle(page, x + 100, y + 40, 40)
  hpdf.Page_ClosePathFillStroke(page)
  hpdf.Page_SetRGBFill(page, 0.0, 0.0, 1.0)
  hpdf.Page_Circle(page, x + 70, y + 74.64, 40)
  hpdf.Page_ClosePathFillStroke(page)
  hpdf.Page_SetRGBFill(page, 0.0, 0.0, 0.0)
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, x + 0.0, y + 130.0, description)
  hpdf.Page_EndText(page)
end -- draw_circles

local pdf = hpdf.New()
if pdf then

  local PAGE_WIDTH = 600
  local PAGE_HEIGHT = 900

  local hfont = hpdf.GetFont(pdf, "Helvetica-Bold")

  -- add a new page object.
  local page = hpdf.AddPage(pdf)

  hpdf.Page_SetFontAndSize(page, hfont, 10)

  hpdf.Page_SetHeight(page, PAGE_HEIGHT)
  hpdf.Page_SetWidth(page, PAGE_WIDTH)

  -- normal
  hpdf.Page_GSave(page)
  draw_circles(page, "normal", 40.0, PAGE_HEIGHT - 170)
  hpdf.Page_GRestore(page)

  -- transparency(0.8)
  hpdf.Page_GSave(page)
  local gstate = hpdf.CreateExtGState(pdf)
  hpdf.ExtGState_SetAlphaFill(gstate, 0.8)
  hpdf.ExtGState_SetAlphaStroke(gstate, 0.8)
  hpdf.Page_SetExtGState(page, gstate)
  draw_circles(page, "alpha fill = 0.8", 230.0, PAGE_HEIGHT - 170)
  hpdf.Page_GRestore(page)

  -- transparency(0.4)
  hpdf.Page_GSave(page)
  gstate = hpdf.CreateExtGState(pdf)
  hpdf.ExtGState_SetAlphaFill(gstate, 0.4)
  hpdf.Page_SetExtGState(page, gstate)
  draw_circles(page, "alpha fill = 0.4", 420.0, PAGE_HEIGHT - 170)
  hpdf.Page_GRestore(page)

  local BlendList = {
    {"HPDF_BM_MULTIPLY", 40, 340},
    {"HPDF_BM_SCREEN", 230, 340},
    {"HPDF_BM_OVERLAY", 420, 340},
    {"HPDF_BM_DARKEN", 40, 510},
    {"HPDF_BM_LIGHTEN", 230, 510},
    {"HPDF_BM_COLOR_DODGE", 420, 510},
    {"HPDF_BM_COLOR_BUM", 40, 680},
    {"HPDF_BM_HARD_LIGHT", 230, 680},
    {"HPDF_BM_SOFT_LIGHT", 420, 680},
    {"HPDF_BM_DIFFERENCE", 40, 850},
    {"HPDF_BM_EXCLUSHON", 230, 850},
  }

  for J, Rec in ipairs(BlendList) do
    hpdf.Page_GSave(page)
    gstate = hpdf.CreateExtGState(pdf)
    hpdf.ExtGState_SetBlendMode(gstate, Rec[1])
    hpdf.Page_SetExtGState(page, gstate)
    draw_circles(page, Rec[1], Rec[2], PAGE_HEIGHT - Rec[3])
    hpdf.Page_GRestore(page)
  end -- J, Rec

  -- save the document to a file
  save_to_file(pdf, arg[0])

  -- clean up
  hpdf.Free(pdf)

  return 0
else
  io.write("Error creating PDF object\n")
end -- if
