--[[--

This script is adpated from grid_sheet.c which has the following header.

Haru Free PDF Library 2.0.0

Copyright (c) 1999-2006 Takeshi Kanno <takeshi_kanno@est.hi-ho.ne.jp>

Permission to use, copy, modify, distribute and sell this software and its
documentation for any purpose is hereby granted without fee, provided that the
above copyright notice appear in all copies and that both that copyright notice
and this permission notice appear in supporting documentation.

It is provided "as is" without express or implied warranty.

--]]--

local hpdf = require "hpdf"

function hpdf.print_grid(pdf, page)
  local height = hpdf.Page_GetHeight(page)
  local width = hpdf.Page_GetWidth(page)
  local font = hpdf.GetFont(pdf, "Helvetica")

  hpdf.Page_SetFontAndSize(page, font, 5)
  hpdf.Page_SetGrayFill(page, 0.5)
  hpdf.Page_SetGrayStroke(page, 0.8)

  -- Draw horizontal lines
  local y = 0
  while y < height do
    if y % 10 == 0 then
      hpdf.Page_SetLineWidth(page, 0.5)
    else
      if hpdf.Page_GetLineWidth(page) ~= 0.25 then
        hpdf.Page_SetLineWidth(page, 0.25)
      end -- if
    end -- if
    hpdf.Page_MoveTo(page, 0, y)
    hpdf.Page_LineTo(page, width, y)
    hpdf.Page_Stroke(page)
    if (y % 10 == 0) and (y > 0) then
      hpdf.Page_SetGrayStroke(page, 0.5)
      hpdf.Page_MoveTo(page, 0, y)
      hpdf.Page_LineTo(page, 5, y)
      hpdf.Page_Stroke(page)
      hpdf.Page_SetGrayStroke(page, 0.8)
    end -- if
    y = y + 5
  end -- while

  -- Draw vertical lines
  local x = 0
  while x < width do
    if x % 10 == 0 then
      hpdf.Page_SetLineWidth(page, 0.5)
    else
      if hpdf.Page_GetLineWidth(page) ~= 0.25 then
        hpdf.Page_SetLineWidth(page, 0.25)
      end -- if
    end -- if
    hpdf.Page_MoveTo(page, x, 0)
    hpdf.Page_LineTo(page, x, height)
    hpdf.Page_Stroke(page)
    if (x % 50 == 0) and (x > 0) then
      hpdf.Page_SetGrayStroke(page, 0.5)
      hpdf.Page_MoveTo(page, x, 0)
      hpdf.Page_LineTo(page, x, 5)
      hpdf.Page_Stroke(page)
      hpdf.Page_MoveTo(page, x, height)
      hpdf.Page_LineTo(page, x, height - 5)
      hpdf.Page_Stroke(page)
      hpdf.Page_SetGrayStroke(page, 0.8)
    end -- if
    x = x + 5
  end -- while

  -- Draw horizontal text
  y = 0
  while y < height do
    if (y % 10 == 0) and (y > 0) then
      hpdf.Page_BeginText(page)
      hpdf.Page_MoveTextPos(page, 5, y - 2)
      hpdf.Page_ShowText(page, y)
      hpdf.Page_EndText(page)
    end -- if
    y = y + 5
  end -- while

  -- Draw vertical text
  x = 0
  while x < width do
    if (x % 50 == 0) and (x > 0) then
      hpdf.Page_BeginText(page)
      hpdf.Page_MoveTextPos(page, x, 5)
      hpdf.Page_ShowText(page, x)
      hpdf.Page_EndText(page)
      hpdf.Page_BeginText(page)
      hpdf.Page_MoveTextPos(page, x, height - 10)
      hpdf.Page_ShowText(page, x)
      hpdf.Page_EndText(page)
    end -- if
    x = x + 5
  end -- while
  hpdf.Page_SetGrayFill(page, 0)
  hpdf.Page_SetGrayStroke(page, 0)

end -- hpdf.print_grid

function save_to_file(pdf, script_name)
  script_name = string.gsub(script_name, "%.%w*$", "") .. ".pdf"
  if 0 ~= hpdf.SaveToFile(pdf, script_name) then
    io.stderr:write('Error writing to ', script_name, '\n')
  end
end

