void PDFDoc::saveIncrementalUpdate (OutStream* outStr)
{
  XRef *uxref;
  int c;
  //copy the original file
  BaseStream *copyStr = str->copy();
  copyStr->reset();
  while ((c = copyStr->getChar()) != EOF) {
    outStr->put(c);
  }
  copyStr->close();
  delete copyStr;

  unsigned char *fileKey;
  CryptAlgorithm encAlgorithm;
  int keyLength;
  xref->getEncryptionParameters(&fileKey, &encAlgorithm, &keyLength);

  uxref = new XRef();
  uxref->add(0, 65535, 0, false);
  xref->lock();
  for(int i=0; i<xref->getNumObjects(); i++) {
    if ((xref->getEntry(i)->type == xrefEntryFree) && 
        (xref->getEntry(i)->gen == 0)) //we skip the irrelevant free objects
      continue;

    if (xref->getEntry(i)->getFlag(XRefEntry::Updated)) { //we have an updated object
      Ref ref;
      ref.num = i;
      ref.gen = xref->getEntry(i)->type == xrefEntryCompressed ? 0 : xref->getEntry(i)->gen;
      if (xref->getEntry(i)->type != xrefEntryFree) {
        Object obj1 = xref->fetch(ref, 1 /* recursion */);
        Goffset offset = writeObjectHeader(&ref, outStr);
        writeObject(&obj1, outStr, fileKey, encAlgorithm, keyLength, ref.num, ref.gen);
        writeObjectFooter(outStr);
        uxref->add(ref.num, ref.gen, offset, true);
      } else {
        uxref->add(ref.num, ref.gen, 0, false);
      }
    }
  }
  xref->unlock();
  // because of "uxref->add(0, 65535, 0, false);" uxref->getNumObjects() will
  // always be >= 1; if it is 1, it means there is nothing to update
  if (uxref->getNumObjects() == 1) {
    delete uxref;
    return;
  }

  Goffset uxrefOffset = outStr->getPos();
  int numobjects = xref->getNumObjects();
  const char *fileNameA = fileName ? fileName->c_str() : nullptr;
  Ref rootRef, uxrefStreamRef;
  rootRef.num = getXRef()->getRootNum();
  rootRef.gen = getXRef()->getRootGen();

  // Output a xref stream if there is a xref stream already
  bool xRefStream = xref->isXRefStream();

  if (xRefStream) {
    // Append an entry for the xref stream itself
    uxrefStreamRef.num = numobjects++;
    uxrefStreamRef.gen = 0;
    uxref->add(uxrefStreamRef.num, uxrefStreamRef.gen, uxrefOffset, true);
  }

  Object trailerDict = createTrailerDict(numobjects, true, getStartXRef(), &rootRef, getXRef(), fileNameA, uxrefOffset);
  if (xRefStream) {
    writeXRefStreamTrailer(std::move(trailerDict), uxref, &uxrefStreamRef, uxrefOffset, outStr, getXRef());
  } else {
    writeXRefTableTrailer(std::move(trailerDict), uxref, false, uxrefOffset, outStr, getXRef());
  }

  delete uxref;
}