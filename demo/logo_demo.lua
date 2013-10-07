local hpdf = require "hpdf"
require "common"

local function logodraw(pdf, page, width, left, bottom)
  local scale = 1 - math.sqrt(2) / 2
  local radius_lg = width / 3
  local diamter_lg = 2 * radius_lg
  local radius_sm = radius_lg * (1 - math.sqrt(2) / 2)
  local diameter_sm = 2 * radius_sm
  local ctr_x = left + (width + radius_sm / 2) / 2
  local ctr_y = bottom + (width + radius_sm / 2) / 2

  hpdf.Page_SetRGBFill(page, 0.125, 0.125, 0.5)

  -- Planet
  hpdf.Page_Circle(page, ctr_x, ctr_y, radius_lg)

  -- Moon
  hpdf.Page_Circle(page, ctr_x + radius_lg, ctr_y + radius_lg, radius_sm)
  hpdf.Page_Fill(page)

  -- Moonshadow
  hpdf.Page_SetRGBFill(page, 1, 1, 1) -- Text and areas
  hpdf.Page_Circle(page, ctr_x + radius_lg - diameter_sm,
    ctr_y + radius_lg - diameter_sm, radius_sm)
  hpdf.Page_Fill(page)

  -- Orbit
  hpdf.Page_SetRGBStroke(page, 0.75, 0.75, 0.75) -- Lines
  hpdf.Page_Arc(page, ctr_x, ctr_y, radius_lg + radius_sm, 0, 30)
  hpdf.Page_Arc(page, ctr_x, ctr_y, radius_lg + radius_sm, 60, 360)
  hpdf.Page_Stroke(page)

  -- Text

  local font = hpdf.GetFont(pdf, "Helvetica")
  local str = "Lua"
  hpdf.Page_SetFontAndSize(page, font, width / 4.6)
  local wd = hpdf.Page_TextWidth(page, str)
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, ctr_x - wd / 2, ctr_y - diameter_sm, str)
  hpdf.Page_EndText(page)

end -- logodraw

local pdf = hpdf.New()
if pdf then
  -- Add a new page object.
  local page = hpdf.AddPage(pdf)
  local height = hpdf.Page_GetHeight(page)
  local width = hpdf.Page_GetWidth(page)
  local edge = width / 2
  logodraw(pdf, page, edge, (width - edge) / 2, (height - edge) / 2)
  save_to_file(pdf, arg[0])
  hpdf.Free(pdf)
else
  io.write("Error creating PDF object\n")
end -- if
