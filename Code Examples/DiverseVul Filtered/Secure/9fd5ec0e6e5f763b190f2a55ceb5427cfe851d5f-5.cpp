void PDFDoc::init()
{
  ok = false;
  errCode = errNone;
  fileName = nullptr;
  file = nullptr;
  str = nullptr;
  xref = nullptr;
  linearization = nullptr;
  catalog = nullptr;
  hints = nullptr;
  outline = nullptr;
  startXRefPos = -1;
  secHdlr = nullptr;
  pageCache = nullptr;
}