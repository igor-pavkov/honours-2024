Outline *PDFDoc::getOutline()
{
  if (!outline) {
    pdfdocLocker();
    // read outline
    outline = new Outline(catalog->getOutline(), xref);
  }

  return outline;
}