unsigned int PDFDoc::writePageObjects(OutStream *outStr, XRef *xRef, unsigned int numOffset, bool combine) 
{
  unsigned int objectsCount = 0; //count the number of objects in the XRef(s)
  unsigned char *fileKey;
  CryptAlgorithm encAlgorithm;
  int keyLength;
  xRef->getEncryptionParameters(&fileKey, &encAlgorithm, &keyLength);

  for (int n = numOffset; n < xRef->getNumObjects(); n++) {
    if (xRef->getEntry(n)->type != xrefEntryFree) {
      Ref ref;
      ref.num = n;
      ref.gen = xRef->getEntry(n)->gen;
      objectsCount++;
      Object obj = getXRef()->fetch(ref.num - numOffset, ref.gen);
      Goffset offset = writeObjectHeader(&ref, outStr);
      if (combine) {
        writeObject(&obj, outStr, getXRef(), numOffset, nullptr, cryptRC4, 0, 0, 0);
      } else if (xRef->getEntry(n)->getFlag(XRefEntry::Unencrypted)) {
        writeObject(&obj, outStr, nullptr, cryptRC4, 0, 0, 0);
      } else {
        writeObject(&obj, outStr, fileKey, encAlgorithm, keyLength, ref.num, ref.gen);
      }
      writeObjectFooter(outStr);
      xRef->add(ref.num, ref.gen, offset, true);
    }
  }
  return objectsCount;
}