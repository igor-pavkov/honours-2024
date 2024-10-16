void PDFDoc::markAcroForm(Object *afObj, XRef *xRef, XRef *countRef, unsigned int numOffset, int oldRefNum, int newRefNum) {
  bool modified = false;
  Object acroform = afObj->fetch(getXRef());
  if (acroform.isDict()) {
      Dict *dict = acroform.getDict();
      for (int i=0; i < dict->getLength(); i++) {
        if (strcmp(dict->getKey(i), "Fields") == 0) {
          Object fields = dict->getValNF(i).copy();
          modified = markAnnotations(&fields, xRef, countRef, numOffset, oldRefNum, newRefNum);
        } else {
          Object obj = dict->getValNF(i).copy();
          markObject(&obj, xRef, countRef, numOffset, oldRefNum, newRefNum);
        }
      }
  }
  if (afObj->isRef()) {
    if (afObj->getRef().num + (int) numOffset >= xRef->getNumObjects() || xRef->getEntry(afObj->getRef().num + numOffset)->type == xrefEntryFree) {
      if (getXRef()->getEntry(afObj->getRef().num)->type == xrefEntryFree) {
        return;  // already marked as free => should be replaced
      }
      xRef->add(afObj->getRef().num + numOffset, afObj->getRef().gen, 0, true);
      if (getXRef()->getEntry(afObj->getRef().num)->type == xrefEntryCompressed) {
        xRef->getEntry(afObj->getRef().num + numOffset)->type = xrefEntryCompressed;
      }
    }
    if (afObj->getRef().num + (int) numOffset >= countRef->getNumObjects() || 
        countRef->getEntry(afObj->getRef().num + numOffset)->type == xrefEntryFree)
    {
      countRef->add(afObj->getRef().num + numOffset, 1, 0, true);
    } else {
      XRefEntry *entry = countRef->getEntry(afObj->getRef().num + numOffset);
      entry->gen++;
    } 
    if (modified){
      getXRef()->setModifiedObject(&acroform, afObj->getRef());
    }
  }
  return;
}