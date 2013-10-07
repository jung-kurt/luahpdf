--[[--

This script is adpated from line_demo.c which has the following header.

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

local function draw_line(page, x, y, label)
  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, x, y - 10)
  hpdf.Page_ShowText(page, label)
  hpdf.Page_EndText(page)

  hpdf.Page_MoveTo(page, x, y - 15)
  hpdf.Page_LineTo(page, x + 220, y - 15)
  hpdf.Page_Stroke(page)
end -- draw_line

local function draw_line2(page, x, y, label)
  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, x, y)
  hpdf.Page_ShowText(page, label)
  hpdf.Page_EndText(page)

  hpdf.Page_MoveTo(page, x + 30, y - 25)
  hpdf.Page_LineTo(page, x + 160, y - 25)
  hpdf.Page_Stroke(page)
end -- draw_line2

local function draw_rect(page, x, y, label)
    hpdf.Page_BeginText(page)
    hpdf.Page_MoveTextPos(page, x, y - 10)
    hpdf.Page_ShowText(page, label)
    hpdf.Page_EndText(page)

    hpdf.Page_Rectangle(page, x, y - 40, 220, 25)
end -- draw_rect

local pdf = hpdf.New()
if pdf then

  local page_title = "Line Example"
  local DASH_MODE1 = {3}
  local DASH_MODE2 = {3, 7}
  local DASH_MODE3 = {8, 7, 2, 7}


  -- create default-font
  local font = hpdf.GetFont(pdf, "Helvetica")

  -- add a new page object
  local page = hpdf.AddPage(pdf)

  -- print the lines of the page
  hpdf.Page_SetLineWidth(page, 1)
  hpdf.Page_Rectangle(page, 50, 50, hpdf.Page_GetWidth(page) - 100,
    hpdf.Page_GetHeight(page) - 110)
  hpdf.Page_Stroke(page)

  -- print the title of the page(with positioning center)
  hpdf.Page_SetFontAndSize(page, font, 24)
  local tw = hpdf.Page_TextWidth(page, page_title)
  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, (hpdf.Page_GetWidth(page) - tw) / 2,
    hpdf.Page_GetHeight(page) - 50)
  hpdf.Page_ShowText(page, page_title)
  hpdf.Page_EndText(page)

  hpdf.Page_SetFontAndSize(page, font, 10)

  -- Draw verious widths of lines
  hpdf.Page_SetLineWidth(page, 0)
  draw_line(page, 60, 770, "line width = 0")

  hpdf.Page_SetLineWidth(page, 1.0)
  draw_line(page, 60, 740, "line width = 1.0")

  hpdf.Page_SetLineWidth(page, 2.0)
  draw_line(page, 60, 710, "line width = 2.0")

  -- Line dash pattern
  hpdf.Page_SetLineWidth(page, 1.0)

  hpdf.Page_SetDash(page, DASH_MODE1, #DASH_MODE1, 1)
  draw_line(page, 60, 680, "dash_ptn=[3], phase=1 -- 2 on, 3 off, 3 on...")

  hpdf.Page_SetDash(page, DASH_MODE2, #DASH_MODE2, 2)
  draw_line(page, 60, 650, "dash_ptn=[7, 3], phase=2 -- 5 on 3 off, 7 on,...")

  hpdf.Page_SetDash(page, DASH_MODE3, #DASH_MODE3, 0)
  draw_line(page, 60, 620, "dash_ptn=[8, 7, 2, 7], phase=0")

  hpdf.Page_SetDash(page, nil, 0, 0)

  hpdf.Page_SetLineWidth(page, 30)
  hpdf.Page_SetRGBStroke(page, 0.0, 0.5, 0.0)

  -- Line Cap Style
  hpdf.Page_SetLineCap(page, "HPDF_BUTT_END")
  draw_line2(page, 60, 570, "HPDF_BUTT_END")

  hpdf.Page_SetLineCap(page, "HPDF_ROUND_END")
  draw_line2(page, 60, 505, "HPDF_ROUND_END")

  hpdf.Page_SetLineCap(page, "HPDF_PROJECTING_SCUARE_END")
  draw_line2(page, 60, 440, "HPDF_PROJECTING_SCUARE_END")

  -- Line Join Style
  hpdf.Page_SetLineWidth(page, 30)
  hpdf.Page_SetRGBStroke(page, 0.0, 0.0, 0.5)

  hpdf.Page_SetLineJoin(page, "HPDF_MITER_JOIN")
  hpdf.Page_MoveTo(page, 120, 300)
  hpdf.Page_LineTo(page, 160, 340)
  hpdf.Page_LineTo(page, 200, 300)
  hpdf.Page_Stroke(page)

  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, 60, 360)
  hpdf.Page_ShowText(page, "HPDF_MITER_JOIN")
  hpdf.Page_EndText(page)

  hpdf.Page_SetLineJoin(page, "HPDF_ROUND_JOIN")
  hpdf.Page_MoveTo(page, 120, 195)
  hpdf.Page_LineTo(page, 160, 235)
  hpdf.Page_LineTo(page, 200, 195)
  hpdf.Page_Stroke(page)

  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, 60, 255)
  hpdf.Page_ShowText(page, "HPDF_ROUND_JOIN")
  hpdf.Page_EndText(page)

  hpdf.Page_SetLineJoin(page, "HPDF_BEVEL_JOIN")
  hpdf.Page_MoveTo(page, 120, 90)
  hpdf.Page_LineTo(page, 160, 130)
  hpdf.Page_LineTo(page, 200, 90)
  hpdf.Page_Stroke(page)

  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, 60, 150)
  hpdf.Page_ShowText(page, "HPDF_BEVEL_JOIN")
  hpdf.Page_EndText(page)

  -- Draw Rectangle
  hpdf.Page_SetLineWidth(page, 2)
  hpdf.Page_SetRGBStroke(page, 0, 0, 0)
  hpdf.Page_SetRGBFill(page, 0.75, 0.0, 0.0)

  draw_rect(page, 300, 770, "Stroke")
  hpdf.Page_Stroke(page)

  draw_rect(page, 300, 720, "Fill")
  hpdf.Page_Fill(page)

  draw_rect(page, 300, 670, "Fill then Stroke")
  hpdf.Page_FillStroke(page)

  -- Clip Rect
  hpdf.Page_GSave(page)  -- Save the current graphic state
  draw_rect(page, 300, 620, "Clip Rectangle")
  hpdf.Page_Clip(page)
  hpdf.Page_Stroke(page)
  hpdf.Page_SetFontAndSize(page, font, 13)

  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, 290, 600)
  hpdf.Page_SetTextLeading(page, 12)
  hpdf.Page_ShowText(page,
    "Clip Clip Clip Clip Clip Clipi Clip Clip Clip")
  hpdf.Page_ShowTextNextLine(page,
    "Clip Clip Clip Clip Clip Clip Clip Clip Clip")
  hpdf.Page_ShowTextNextLine(page,
    "Clip Clip Clip Clip Clip Clip Clip Clip Clip")
  hpdf.Page_EndText(page)
  hpdf.Page_GRestore(page)

  -- Curve Example(CurveTo2)
  local x = 330
  local y = 440
  local x1 = 430
  local y1 = 530
  local x2 = 480
  local y2 = 470
  local x3 = 480
  local y3 = 90

  hpdf.Page_SetRGBFill(page, 0, 0, 0)

  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, 300, 540)
  hpdf.Page_ShowText(page, "CurveTo2(x1, y1, x2. y2)")
  hpdf.Page_EndText(page)

  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, x + 5, y - 5)
  hpdf.Page_ShowText(page, "Current point")
  hpdf.Page_MoveTextPos(page, x1 - x, y1 - y)
  hpdf.Page_ShowText(page, "(x1, y1)")
  hpdf.Page_MoveTextPos(page, x2 - x1, y2 - y1)
  hpdf.Page_ShowText(page, "(x2, y2)")
  hpdf.Page_EndText(page)

  hpdf.Page_SetDash(page, DASH_MODE1, #DASH_MODE1, 0)

  hpdf.Page_SetLineWidth(page, 0.5)
  hpdf.Page_MoveTo(page, x1, y1)
  hpdf.Page_LineTo(page, x2, y2)
  hpdf.Page_Stroke(page)

  hpdf.Page_SetDash(page)

  hpdf.Page_SetLineWidth(page, 1.5)

  hpdf.Page_MoveTo(page, x, y)
  hpdf.Page_CurveTo2(page, x1, y1, x2, y2)
  hpdf.Page_Stroke(page)

  -- Curve Example(CurveTo3)
  y = y - 150
  y1 = y1 - 150
  y2 = y2 - 150

  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, 300, 390)
  hpdf.Page_ShowText(page, "CurveTo3(x1, y1, x2. y2)")
  hpdf.Page_EndText(page)

  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, x + 5, y - 5)
  hpdf.Page_ShowText(page, "Current point")
  hpdf.Page_MoveTextPos(page, x1 - x, y1 - y)
  hpdf.Page_ShowText(page, "(x1, y1)")
  hpdf.Page_MoveTextPos(page, x2 - x1, y2 - y1)
  hpdf.Page_ShowText(page, "(x2, y2)")
  hpdf.Page_EndText(page)

  hpdf.Page_SetDash(page, DASH_MODE1, #DASH_MODE1, 0)

  hpdf.Page_SetLineWidth(page, 0.5)
  hpdf.Page_MoveTo(page, x, y)
  hpdf.Page_LineTo(page, x1, y1)
  hpdf.Page_Stroke(page)

  hpdf.Page_SetDash(page)

  hpdf.Page_SetLineWidth(page, 1.5)
  hpdf.Page_MoveTo(page, x, y)
  hpdf.Page_CurveTo3(page, x1, y1, x2, y2)
  hpdf.Page_Stroke(page)

  -- Curve Example(CurveTo)
  y = y - 150
  y1 = y1 - 160
  y2 = y2 - 130
  x2 = x2 + 10

  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, 300, 240)
  hpdf.Page_ShowText(page, "CurveTo(x1, y1, x2. y2, x3, y3)")
  hpdf.Page_EndText(page)

  hpdf.Page_BeginText(page)
  hpdf.Page_MoveTextPos(page, x + 5, y - 5)
  hpdf.Page_ShowText(page, "Current point")
  hpdf.Page_MoveTextPos(page, x1 - x, y1 - y)
  hpdf.Page_ShowText(page, "(x1, y1)")
  hpdf.Page_MoveTextPos(page, x2 - x1, y2 - y1)
  hpdf.Page_ShowText(page, "(x2, y2)")
  hpdf.Page_MoveTextPos(page, x3 - x2, y3 - y2)
  hpdf.Page_ShowText(page, "(x3, y3)")
  hpdf.Page_EndText(page)

  hpdf.Page_SetDash(page, DASH_MODE1, #DASH_MODE1, 0)

  hpdf.Page_SetLineWidth(page, 0.5)
  hpdf.Page_MoveTo(page, x, y)
  hpdf.Page_LineTo(page, x1, y1)
  hpdf.Page_Stroke(page)
  hpdf.Page_MoveTo(page, x2, y2)
  hpdf.Page_LineTo(page, x3, y3)
  hpdf.Page_Stroke(page)

  hpdf.Page_SetDash(page)

  hpdf.Page_SetLineWidth(page, 1.5)
  hpdf.Page_MoveTo(page, x, y)
  hpdf.Page_CurveTo(page, x1, y1, x2, y2, x3, y3)
  hpdf.Page_Stroke(page)

  -- save the document to a file
  save_to_file(pdf, arg[0])

  -- clean up
  hpdf.Free(pdf)

  return 0
else
  io.write("Error creating PDF object\n")
end -- if
