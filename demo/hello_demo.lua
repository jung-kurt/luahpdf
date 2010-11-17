require "hpdf"

local pdf = hpdf.New()
if pdf then
  local page = hpdf.AddPage(pdf)
  -- hpdf.Page_SetWidth(page, 72)
  -- hpdf.Page_SetHeight(page, 72)
  local height = hpdf.Page_GetHeight(page)
  local width = hpdf.Page_GetWidth(page)
  -- io.write("width ", width, ", height ", height, "\n")
  local font = hpdf.GetFont(pdf, "Times-Roman")
  hpdf.Page_SetFontAndSize(page, font, 24)
  hpdf.Page_BeginText(page)
  hpdf.Page_TextOut(page, 60, height - 60, "Hello from Haru version " ..
    hpdf.VERSION_TEXT)
  hpdf.Page_EndText(page)
  hpdf.SaveToFile(pdf, string.gsub(arg[0], "%.%w*$", "") .. ".pdf")
  hpdf.Free(pdf)
else
  io.write("Error creating PDF object\n")
end -- if
