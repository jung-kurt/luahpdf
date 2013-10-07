--[[--

This script is adpated from permission.c which has the following header.

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

local text = "User cannot print and copy this document."
local owner_passwd = "owner"
local user_passwd = ""

local pdf = hpdf.New()
if pdf then

  -- create default-font
  local font = hpdf.GetFont(pdf, "Helvetica")

  -- add a new page object
  local page = hpdf.AddPage(pdf)

  hpdf.Page_SetSize(page, "HPDF_PAGE_SIZE_B5", "HPDF_PAGE_LANDSCAPE")

  hpdf.Page_BeginText(page)
  hpdf.Page_SetFontAndSize(page, font, 20)
  local tw = hpdf.Page_TextWidth(page, text)
  hpdf.Page_MoveTextPos(page, (hpdf.Page_GetWidth(page) - tw) / 2,
    (hpdf.Page_GetHeight(page)  - 20) / 2)
  hpdf.Page_ShowText(page, text)
  hpdf.Page_EndText(page)

  hpdf.SetPassword(pdf, owner_passwd, user_passwd)
  hpdf.SetPermission(pdf, hpdf.ENABLE_READ)
  hpdf.SetEncryptionMode(pdf, "HPDF_ENCRYPT_R3", 16)

  -- save the document to a file
  save_to_file(pdf, arg[0])

  -- clean up
  hpdf.Free(pdf)

  return 0
else
  io.write("Error creating PDF object\n")
end -- if
