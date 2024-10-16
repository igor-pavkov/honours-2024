void PDFDoc::markPageObjects(Dict *pageDict, XRef *xRef, XRef *countRef, unsigned int numOffset, int oldRefNum, int newRefNum, std::set<Dict*> *alreadyMarkedDicts)
{
  pageDict->remove("OpenAction");
  pageDict->remove("Outlines");
  pageDict->remove("StructTreeRoot");

  for (int n = 0; n < pageDict->getLength(); n++) {
    const char *key = pageDict->getKey(n);
    Object value  = pageDict->getValNF(n).copy();
    if (strcmp(key, "Parent") != 0 &&
	      strcmp(key, "Pages") != 0 &&
	      strcmp(key, "AcroForm") != 0 &&
	      strcmp(key, "Annots") != 0 &&
	      strcmp(key, "P") != 0 &&
        strcmp(key, "Root") != 0) {
      markObject(&value, xRef, countRef, numOffset, oldRefNum, newRefNum, alreadyMarkedDicts);
    }
  }
}