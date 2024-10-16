void PDFDoc::markDictionnary (Dict* dict, XRef * xRef, XRef *countRef, unsigned int numOffset, int oldRefNum, int newRefNum, std::set<Dict*> *alreadyMarkedDicts)
{
  bool deleteSet = false;
  if (!alreadyMarkedDicts) {
    alreadyMarkedDicts = new std::set<Dict*>;
    deleteSet = true;
  }

  if (alreadyMarkedDicts->find(dict) != alreadyMarkedDicts->end()) {
    error(errSyntaxWarning, -1, "PDFDoc::markDictionnary: Found recursive dicts");
    if (deleteSet) delete alreadyMarkedDicts;
    return;
  } else {
    alreadyMarkedDicts->insert(dict);
  }

  for (int i=0; i<dict->getLength(); i++) {
    const char *key = dict->getKey(i);
    if (strcmp(key, "Annots") != 0) {
      Object obj1 = dict->getValNF(i).copy();
      markObject(&obj1, xRef, countRef, numOffset, oldRefNum, newRefNum, alreadyMarkedDicts);
    } else {
      Object annotsObj = dict->getValNF(i).copy();
      if (!annotsObj.isNull()) {
        markAnnotations(&annotsObj, xRef, countRef, 0, oldRefNum, newRefNum, alreadyMarkedDicts);
      }
    }
  }

  if (deleteSet) {
    delete alreadyMarkedDicts;
  }
}