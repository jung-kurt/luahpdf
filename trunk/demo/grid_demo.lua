local hpdf = require "hpdf"
require "common"

local pdf = hpdf.New()
if pdf then
  local page = hpdf.AddPage(pdf)
  hpdf.print_grid(pdf, page)
  save_to_file(pdf, arg[0])
  hpdf.Free(pdf)
else
  io.write("Error creating PDF object\n")
end -- if
