bool PDFDoc::checkLinearization() {
  if (linearization == nullptr)
    return false;
  if (linearizationState == 1)
    return true;
  if (linearizationState == 2)
    return false;
  if (!hints) {
    hints = new Hints(str, linearization, getXRef(), secHdlr);
  }
  if (!hints->isOk()) {
    linearizationState = 2;
    return false;
  }
  for (int page = 1; page <= linearization->getNumPages(); page++) {
    Ref pageRef;

    pageRef.num = hints->getPageObjectNum(page);
    if (!pageRef.num) {
      linearizationState = 2;
      return false;
    }

    // check for bogus ref - this can happen in corrupted PDF files
    if (pageRef.num < 0 || pageRef.num >= xref->getNumObjects()) {
      linearizationState = 2;
      return false;
    }

    pageRef.gen = xref->getEntry(pageRef.num)->gen;
    Object obj = xref->fetch(pageRef);
    if (!obj.isDict("Page")) {
      linearizationState = 2;
      return false;
    }
  }
  linearizationState = 1;
  return true;
}