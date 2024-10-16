void PDFDoc::processLinks(OutputDev *out, int page) {
  if (getPage(page))
    getPage(page)->processLinks(out);
}