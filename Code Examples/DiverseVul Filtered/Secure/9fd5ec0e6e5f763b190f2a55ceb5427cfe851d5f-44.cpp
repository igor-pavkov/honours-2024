GooString *PDFDoc::getDocInfoStringEntry(const char *key) {
  Object infoObj = getDocInfo();
  if (!infoObj.isDict()) {
      return nullptr;
  }

  Object entryObj = infoObj.dictLookup(key);

  GooString *result;

  if (entryObj.isString()) {
    result = entryObj.takeString();
  } else {
    result = nullptr;
  }

  return result;
}