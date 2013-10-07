local hpdf = require "hpdf"
require "common"

local pdf = hpdf.New()
if pdf then
  local page = hpdf.AddPage(pdf)
  local height = hpdf.Page_GetHeight(page)
  local width = hpdf.Page_GetWidth(page)
  local font = hpdf.GetFont(pdf, "Times-Roman")
  hpdf.Page_SetFontAndSize(page, font, 24)
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, 60, height - 60, "Hello from Haru version " ..
    hpdf.VERSION_TEXT)
  hpdf.Page_EndText(page)
  save_to_file(pdf, arg[0])
  hpdf.Free(pdf)
else
  io.write("Error creating PDF object\n")
end -- if
