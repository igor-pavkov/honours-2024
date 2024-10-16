int PDFDoc::savePageAs(const GooString *name, int pageNo)
{
  FILE *f;
  OutStream *outStr;
  XRef *yRef, *countRef;

  if (file && file->modificationTimeChangedSinceOpen())
    return errFileChangedSinceOpen;


  int rootNum = getXRef()->getNumObjects() + 1;

  // Make sure that special flags are set, because we are going to read
  // all objects, including Unencrypted ones.
  xref->scanSpecialFlags();

  unsigned char *fileKey;
  CryptAlgorithm encAlgorithm;
  int keyLength;
  xref->getEncryptionParameters(&fileKey, &encAlgorithm, &keyLength);

  if (pageNo < 1 || pageNo > getNumPages() || !getCatalog()->getPage(pageNo)) {
    error(errInternal, -1, "Illegal pageNo: {0:d}({1:d})", pageNo, getNumPages() );
    return errOpenFile;
  }
  const PDFRectangle *cropBox = nullptr;
  if (getCatalog()->getPage(pageNo)->isCropped()) {
    cropBox = getCatalog()->getPage(pageNo)->getCropBox();
  }
  replacePageDict(pageNo, 
    getCatalog()->getPage(pageNo)->getRotate(),
    getCatalog()->getPage(pageNo)->getMediaBox(),
    cropBox);
  Ref *refPage = getCatalog()->getPageRef(pageNo);
  Object page = getXRef()->fetch(*refPage);

  if (!(f = openFile(name->c_str(), "wb"))) {
    error(errIO, -1, "Couldn't open file '{0:t}'", name);
    return errOpenFile;
  }
  outStr = new FileOutStream(f,0);

  yRef = new XRef(getXRef()->getTrailerDict());

  if (secHdlr != nullptr && !secHdlr->isUnencrypted()) {
    yRef->setEncryption(secHdlr->getPermissionFlags(), 
      secHdlr->getOwnerPasswordOk(), fileKey, keyLength, secHdlr->getEncVersion(), secHdlr->getEncRevision(), encAlgorithm);
  }
  countRef = new XRef();
  Object *trailerObj = getXRef()->getTrailerDict();
  if (trailerObj->isDict()) {
    markPageObjects(trailerObj->getDict(), yRef, countRef, 0, refPage->num, rootNum + 2);
  }
  yRef->add(0, 65535, 0, false);
  writeHeader(outStr, getPDFMajorVersion(), getPDFMinorVersion());

  // get and mark info dict
  Object infoObj = getXRef()->getDocInfo();
  if (infoObj.isDict()) {
    Dict *infoDict = infoObj.getDict();
    markPageObjects(infoDict, yRef, countRef, 0, refPage->num, rootNum + 2);
    if (trailerObj->isDict()) {
      Dict *trailerDict = trailerObj->getDict();
      const Object &ref = trailerDict->lookupNF("Info");
      if (ref.isRef()) {
        yRef->add(ref.getRef().num, ref.getRef().gen, 0, true);
        if (getXRef()->getEntry(ref.getRef().num)->type == xrefEntryCompressed) {
          yRef->getEntry(ref.getRef().num)->type = xrefEntryCompressed;
        }
      }
    }
  }
  
  // get and mark output intents etc.
  Object catObj = getXRef()->getCatalog();
  Dict *catDict = catObj.getDict();
  Object pagesObj = catDict->lookup("Pages");
  Object afObj = catDict->lookupNF("AcroForm").copy();
  if (!afObj.isNull()) {
    markAcroForm(&afObj, yRef, countRef, 0, refPage->num, rootNum + 2);
  }
  Dict *pagesDict = pagesObj.getDict();
  Object resourcesObj = pagesDict->lookup("Resources");
  if (resourcesObj.isDict())
    markPageObjects(resourcesObj.getDict(), yRef, countRef, 0, refPage->num, rootNum + 2);
  markPageObjects(catDict, yRef, countRef, 0, refPage->num, rootNum + 2);

  Dict *pageDict = page.getDict();
  if (resourcesObj.isNull() && !pageDict->hasKey("Resources")) {
    Object *resourceDictObject = getCatalog()->getPage(pageNo)->getResourceDictObject();
    if (resourceDictObject->isDict()) {
      resourcesObj = resourceDictObject->copy();
      markPageObjects(resourcesObj.getDict(), yRef, countRef, 0, refPage->num, rootNum + 2);
    }
  }
  markPageObjects(pageDict, yRef, countRef, 0, refPage->num, rootNum + 2);
  Object annotsObj = pageDict->lookupNF("Annots").copy();
  if (!annotsObj.isNull()) {
    markAnnotations(&annotsObj, yRef, countRef, 0, refPage->num, rootNum + 2);
  }
  yRef->markUnencrypted();
  writePageObjects(outStr, yRef, 0);

  yRef->add(rootNum,0,outStr->getPos(),true);
  outStr->printf("%d 0 obj\n", rootNum);
  outStr->printf("<< /Type /Catalog /Pages %d 0 R", rootNum + 1); 
  for (int j = 0; j < catDict->getLength(); j++) {
    const char *key = catDict->getKey(j);
    if (strcmp(key, "Type") != 0 &&
      strcmp(key, "Catalog") != 0 &&
      strcmp(key, "Pages") != 0) 
    {
      if (j > 0) outStr->printf(" ");
      Object value = catDict->getValNF(j).copy();
      outStr->printf("/%s ", key);
      writeObject(&value, outStr, getXRef(), 0, nullptr, cryptRC4, 0, 0, 0);
    }
  }
  outStr->printf(">>\nendobj\n");

  yRef->add(rootNum + 1,0,outStr->getPos(),true);
  outStr->printf("%d 0 obj\n", rootNum + 1);
  outStr->printf("<< /Type /Pages /Kids [ %d 0 R ] /Count 1 ", rootNum + 2);
  if (resourcesObj.isDict()) {
    outStr->printf("/Resources ");
    writeObject(&resourcesObj, outStr, getXRef(), 0, nullptr, cryptRC4, 0, 0, 0);
  }
  outStr->printf(">>\n");
  outStr->printf("endobj\n");

  yRef->add(rootNum + 2,0,outStr->getPos(),true);
  outStr->printf("%d 0 obj\n", rootNum + 2);
  outStr->printf("<< ");
  for (int n = 0; n < pageDict->getLength(); n++) {
    if (n > 0) outStr->printf(" ");
    const char *key = pageDict->getKey(n);
    Object value = pageDict->getValNF(n).copy();
    if (strcmp(key, "Parent") == 0) {
      outStr->printf("/Parent %d 0 R", rootNum + 1);
    } else {
      outStr->printf("/%s ", key);
      writeObject(&value, outStr, getXRef(), 0, nullptr, cryptRC4, 0, 0, 0);
    }
  }
  outStr->printf(" >>\nendobj\n");

  Goffset uxrefOffset = outStr->getPos();
  Ref ref;
  ref.num = rootNum;
  ref.gen = 0;
  Object trailerDict = createTrailerDict(rootNum + 3, false, 0, &ref, getXRef(),
                                        name->c_str(), uxrefOffset);
  writeXRefTableTrailer(std::move(trailerDict), yRef, false /* do not write unnecessary entries */,
                        uxrefOffset, outStr, getXRef());

  outStr->close();
  fclose(f);
  delete yRef;
  delete countRef;
  delete outStr;

  return errNone;
}