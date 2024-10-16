void PDFDoc::setDocInfoStringEntry(const char *key, GooString *value)
{
  bool removeEntry = !value || value->getLength() == 0 || value->hasJustUnicodeMarker();
  if (removeEntry) {
    delete value;
  }

  Object infoObj = getDocInfo();
  if (infoObj.isNull() && removeEntry) {
    // No info dictionary, so no entry to remove.
    return;
  }

  infoObj = createDocInfoIfNoneExists();
  if (removeEntry) {
    infoObj.dictSet(key, Object(objNull));
  } else {
    infoObj.dictSet(key, Object(value));
  }


  if (infoObj.dictGetLength() == 0) {
    // Info dictionary is empty. Remove it altogether.
    removeDocInfo();
  } else {
    setDocInfoModified(&infoObj);
  }
}