Links *PDFDoc::getLinks(int page) {
  Page *p = getPage(page);
  if (!p) {
    return new Links (nullptr);
  }
  return p->getLinks();
}