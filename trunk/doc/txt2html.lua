package.path = 'doc/?.lua;./?.lua'
require "markdown"

local HdrStr = [[
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">

<html lang="en">

<head>
  <title>LuaHPDF</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
  <link href="default.css" type="text/css" rel="stylesheet">
</head>

<body>
]]

local FtrStr = [[

  <div class="footer">

  <p>Copyright &copy; 2007,2013 Kurt Jung</p>

  </div>

</body>

</html>
]]

io.write(HdrStr, markdown(io.read("*all") or ""), FtrStr)
