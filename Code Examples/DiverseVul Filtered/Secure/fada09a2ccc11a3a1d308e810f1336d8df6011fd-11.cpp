Page *PDFDoc::parsePage(int page)
{
  Ref pageRef;

  pageRef.num = getHints()->getPageObjectNum(page);
  if (!pageRef.num) {
    error(errSyntaxWarning, -1, "Failed to get object num from hint tables for page {0:d}", page);
    return nullptr;
  }

  // check for bogus ref - this can happen in corrupted PDF files
  if (pageRef.num < 0 || pageRef.num >= xref->getNumObjects()) {
    error(errSyntaxWarning, -1, "Invalid object num ({0:d}) for page {1:d}", pageRef.num, page);
    return nullptr;
  }

  pageRef.gen = xref->getEntry(pageRef.num)->gen;
  Object obj = xref->fetch(pageRef);
  if (!obj.isDict("Page")) {
    error(errSyntaxWarning, -1, "Object ({0:d} {1:d}) is not a pageDict", pageRef.num, pageRef.gen);
    return nullptr;
  }
  Dict *pageDict = obj.getDict();

  return new Page(this, page, std::move(obj), pageRef,
               new PageAttrs(nullptr, pageDict), catalog->getForm());
}