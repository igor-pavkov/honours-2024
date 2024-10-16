Page *PDFDoc::getPage(int page)
{
  if ((page < 1) || page > getNumPages()) return nullptr;

  if (isLinearized() && checkLinearization()) {
    pdfdocLocker();
    if (!pageCache) {
      pageCache = (Page **) gmallocn(getNumPages(), sizeof(Page *));
      for (int i = 0; i < getNumPages(); i++) {
        pageCache[i] = nullptr;
      }
    }
    if (!pageCache[page-1]) {
      pageCache[page-1] = parsePage(page);
    }
    if (pageCache[page-1]) {
       return pageCache[page-1];
    } else {
       error(errSyntaxWarning, -1, "Failed parsing page {0:d} using hint tables", page);
    }
  }

  return catalog->getPage(page);
}