require "hpdf"
require "grid_sheet"

local pdf = hpdf.New()
if pdf then
  local page = hpdf.AddPage(pdf)
  hpdf.print_grid(pdf, page)
  hpdf.SaveToFile(pdf, string.gsub(arg[0], "%.%w*$", "") .. ".pdf")
  hpdf.Free(pdf)
else
  io.write("Error creating PDF object\n")
end -- if
