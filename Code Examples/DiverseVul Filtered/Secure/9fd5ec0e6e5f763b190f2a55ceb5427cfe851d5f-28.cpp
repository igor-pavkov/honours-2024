Hints *PDFDoc::getHints()
{
  if (!hints && isLinearized()) {
    hints = new Hints(str, getLinearization(), getXRef(), secHdlr);
  }

  return hints;
}