--[[--

This script is adpated from text_demo.c which has the following header.

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

local function show_stripe_pattern(page, x, y)

  local iy = 0
  while iy < 50 do
    hpdf.Page_SetRGBStroke(page, 0.0, 0.0, 0.5)
    hpdf.Page_SetLineWidth(page, 1)
    hpdf.Page_MoveTo(page, x, y + iy)
    hpdf.Page_LineTo(page, x + hpdf.Page_TextWidth(page, "ABCabc123"), y + iy)
    hpdf.Page_Stroke(page)
    iy = iy + 3
  end -- while
  hpdf.Page_SetLineWidth(page, 2.5)
end -- show_stripe_pattern

local function show_description(page, x, y, text)

  local fsize = hpdf.Page_GetCurrentFontSize(page)
  local font = hpdf.Page_GetCurrentFont(page)
  local c = hpdf.Page_GetRGBFill(page)
  hpdf.Page_BeginText(page)
  hpdf.Page_SetRGBFill(page, 0, 0, 0)
  hpdf.Page_SetTextRenderingMode(page, "HPDF_FILL")
  hpdf.Page_SetFontAndSize(page, font, 10)
  hpdf.Page_TextOut(page, x, y - 12, text)
  hpdf.Page_EndText(page)
  hpdf.Page_SetFontAndSize(page, font, fsize)
  hpdf.Page_SetRGBFill(page, c.r, c.g, c.b)
end -- show_description

local page_title = "Text Demonstration"
local samp_text = "abcdefgABCDEFG123!#$%&+-@?"
local samp_text2 = "The quick brown fox jumps over the lazy dog."
local pdf = hpdf.New()
if pdf then
  -- set compression mode
  hpdf.SetCompressionMode(pdf, hpdf.COMP_ALL)
  -- create default-font
  local font = hpdf.GetFont(pdf, "Helvetica")
  -- add a new page object
  local page = hpdf.AddPage(pdf)
  -- draw grid to the page
  hpdf.print_grid (pdf, page)
  -- print the title of the page(with positioning center)
  hpdf.Page_SetFontAndSize(page, font, 24)
  local tw = hpdf.Page_TextWidth(page, page_title)

  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page,(hpdf.Page_GetWidth(page) - tw) / 2,
    hpdf.Page_GetHeight(page) - 50, page_title)
  hpdf.Page_EndText(page)

  hpdf.Page_BeginText(page)
  local margin = 60
  hpdf.Page_MoveTextPos(page, margin, hpdf.Page_GetHeight(page) - 60)

  -- font size

  local fsize = 8
  local bound = hpdf.Page_GetWidth(page) - 2 * margin
  while fsize < 60 do
    -- set style and size of font.
    hpdf.Page_SetFontAndSize(page, font, fsize)
    -- set the position of the text.
    hpdf.Page_MoveTextPos(page, 0, -5 - fsize)
    -- measure the number of characters which are included in the page.
    local len = hpdf.Page_MeasureText(page, samp_text, bound, hpdf.FALSE)
    -- truncate the text
    hpdf.Page_ShowText(page, string.sub(samp_text, 1, len))
    -- print the description.
    hpdf.Page_MoveTextPos(page, 0, -10)
    hpdf.Page_SetFontAndSize(page, font, 8)
    hpdf.Page_ShowText(page, string.format("Font size = %.0f", fsize))
    fsize = fsize * 1.5
  end -- while

  -- font color

  hpdf.Page_SetFontAndSize(page, font, 8)
  hpdf.Page_MoveTextPos(page, 0, -30)
  hpdf.Page_ShowText(page, "Font color")

  hpdf.Page_SetFontAndSize(page, font, 18)
  hpdf.Page_MoveTextPos(page, 0, -20)
  local len = string.len(samp_text)
  for J = 1, len do
    local r = J / len
    local g = 1 - J / len
    local buf = string.sub(samp_text, J, J)
    hpdf.Page_SetRGBFill(page, r, g, 0)
    hpdf.Page_ShowText(page, buf)
  end -- J
  hpdf.Page_MoveTextPos(page, 0, -25)

  for J = 1, len do
    local r = J / len
    local b = 1 - J / len
    local buf = string.sub(samp_text, J, J)
    hpdf.Page_SetRGBFill(page, r, 0, b)
    hpdf.Page_ShowText(page, buf)
  end -- J
  hpdf.Page_MoveTextPos(page, 0, -25)

  for J = 1, len do
    local g = J / len
    local b = 1 - J / len
    local buf = string.sub(samp_text, J, J)
    hpdf.Page_SetRGBFill(page, 0, g, b)
    hpdf.Page_ShowText(page, buf)
  end -- J

  hpdf.Page_EndText(page)

  local ypos = 450

  -- Font rendering mode

  hpdf.Page_SetFontAndSize(page, font, 32)
  hpdf.Page_SetRGBFill(page, 0.5, 0.5, 0.0)
  hpdf.Page_SetLineWidth(page, 1.5)

  -- PDF_FILL
  show_description(page,  60, ypos, "RenderingMode=PDF_FILL")
  hpdf.Page_SetTextRenderingMode(page, "HPDF_FILL")
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, 60, ypos, "ABCabc123")
  hpdf.Page_EndText(page)

  -- PDF_STROKE
  show_description(page, 60, ypos - 50, "RenderingMode=PDF_STROKE")
  hpdf.Page_SetTextRenderingMode(page, "HPDF_STROKE")
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, 60, ypos - 50, "ABCabc123")
  hpdf.Page_EndText(page)

  -- PDF_FILL_THEN_STROKE
  show_description(page, 60, ypos - 100, "RenderingMode=PDF_FILL_THEN_STROKE")
  hpdf.Page_SetTextRenderingMode(page, "HPDF_FILL_THEN_STROKE")
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, 60, ypos - 100, "ABCabc123")
  hpdf.Page_EndText(page)

  -- PDF_FILL_CLIPPING
  show_description(page, 60, ypos - 150, "RenderingMode=PDF_FILL_CLIPPING")
  hpdf.Page_GSave(page)
  hpdf.Page_SetTextRenderingMode(page, "HPDF_FILL_CLIPPING")
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, 60, ypos - 150, "ABCabc123")
  hpdf.Page_EndText(page)
  show_stripe_pattern(page, 60, ypos - 150)
  hpdf.Page_GRestore(page)

  -- PDF_STROKE_CLIPPING
  show_description(page, 60, ypos - 200, "RenderingMode=PDF_STROKE_CLIPPING")
  hpdf.Page_GSave(page)
  hpdf.Page_SetTextRenderingMode(page, "HPDF_STROKE_CLIPPING")
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, 60, ypos - 200, "ABCabc123")
  hpdf.Page_EndText(page)
  show_stripe_pattern(page, 60, ypos - 200)
  hpdf.Page_GRestore(page)

  -- PDF_FILL_STROKE_CLIPPING
  show_description(page, 60, ypos - 250,
    "RenderingMode=PDF_FILL_STROKE_CLIPPING")
  hpdf.Page_GSave(page)
  hpdf.Page_SetTextRenderingMode(page, "HPDF_FILL_STROKE_CLIPPING")
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, 60, ypos - 250, "ABCabc123")
  hpdf.Page_EndText(page)
  show_stripe_pattern(page, 60, ypos - 250)
  hpdf.Page_GRestore(page)

  -- Reset text attributes
  hpdf.Page_SetTextRenderingMode(page, "HPDF_FILL")
  hpdf.Page_SetRGBFill(page, 0, 0, 0)
  hpdf.Page_SetFontAndSize(page, font, 30)

  -- Rotating text

  local angle1 = 30 -- A rotation of 30 degrees
  local rad1 = math.rad(angle1)
  show_description(page, 320, ypos - 60, "Rotating text")
  hpdf.Page_BeginText(page)
  hpdf.Page_SetTextMatrix(page, math.cos(rad1), math.sin(rad1),
    -math.sin(rad1), math.cos(rad1), 330, ypos - 60)
  hpdf.Page_ShowText(page, "ABCabc123")
  hpdf.Page_EndText(page)

  -- Skewing text

  show_description(page, 320, ypos - 120, "Skewing text")
  hpdf.Page_BeginText(page)
  angle1 = 10
  local angle2 = 20
  rad1 = math.rad(angle1)
  local rad2 = math.rad(angle2)
  hpdf.Page_SetTextMatrix(page, 1, math.tan(rad1), math.tan(rad2),
    1, 320, ypos - 120)
  hpdf.Page_ShowText(page, "ABCabc123")
  hpdf.Page_EndText(page)

  -- scaling text(X direction)

  show_description(page, 320, ypos - 175, "Scaling text (X direction)")
  hpdf.Page_BeginText(page)
  hpdf.Page_SetTextMatrix(page, 1.5, 0, 0, 1, 320, ypos - 175)
  hpdf.Page_ShowText(page, "ABCabc12")
  hpdf.Page_EndText(page)

  -- scaling text(Y direction)

  show_description(page, 320, ypos - 250, "Scaling text (Y direction)")
  hpdf.Page_BeginText(page)
  hpdf.Page_SetTextMatrix(page, 1, 0, 0, 2, 320, ypos - 250)
  hpdf.Page_ShowText(page, "ABCabc123")
  hpdf.Page_EndText(page)

  -- char spacing, word spacing

  show_description(page, 60, 140, "char-spacing 0")
  show_description(page, 60, 100, "char-spacing 1.5")
  show_description(page, 60, 60, "char-spacing 1.5, word-spacing 2.5")

  hpdf.Page_SetFontAndSize(page, font, 20)
  hpdf.Page_SetRGBFill(page, 0.1, 0.3, 0.1)

  -- char-spacing 0
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, 60, 140, samp_text2)
  hpdf.Page_EndText(page)

  -- char-spacing 1.5
  hpdf.Page_SetCharSpace(page, 1.5)

  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, 60, 100, samp_text2)
  hpdf.Page_EndText(page)

  -- char-spacing 1.5, word-spacing 3.5
  hpdf.Page_SetWordSpace(page, 2.5)

  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, 60, 60, samp_text2)
  hpdf.Page_EndText(page)

  -- save the document to a file
  save_to_file(pdf, arg[0])

  -- clean up
  hpdf.Free(pdf)

  return 0
else
  io.write("Error creating PDF object\n")
end -- if
