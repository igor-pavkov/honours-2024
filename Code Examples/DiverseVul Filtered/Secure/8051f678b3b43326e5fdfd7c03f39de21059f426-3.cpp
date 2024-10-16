std::vector<FontInfo*> *FontInfoScanner::scan(int nPages) {
  Page *page;
  Dict *resDict;
  Annots *annots;
  int lastPage;

  if (currentPage > doc->getNumPages()) {
    return nullptr;
  }
 
  auto result = new std::vector<FontInfo*>();

  lastPage = currentPage + nPages;
  if (lastPage > doc->getNumPages() + 1) {
    lastPage = doc->getNumPages() + 1;
  }

  XRef *xrefA = doc->getXRef()->copy();
  for (int pg = currentPage; pg < lastPage; ++pg) {
    page = doc->getPage(pg);
    if (!page) continue;

    if ((resDict = page->getResourceDictCopy(xrefA))) {
      scanFonts(xrefA, resDict, result);
      delete resDict;
    }
    annots = page->getAnnots();
    for (int i = 0; i < annots->getNumAnnots(); ++i) {
      Object obj1 = annots->getAnnot(i)->getAppearanceResDict();
      if (obj1.isDict()) {
        scanFonts(xrefA, obj1.getDict(), result);
      }
    }
  }

  currentPage = lastPage;

  delete xrefA;
  return result;
}