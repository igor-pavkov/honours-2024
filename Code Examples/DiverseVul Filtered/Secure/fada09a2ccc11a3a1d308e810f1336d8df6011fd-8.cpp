void PDFDoc::writeDictionnary (Dict* dict, OutStream* outStr, XRef *xRef, unsigned int numOffset, unsigned char *fileKey,
                               CryptAlgorithm encAlgorithm, int keyLength, int objNum, int objGen, std::set<Dict*> *alreadyWrittenDicts)
{
  bool deleteSet = false;
  if (!alreadyWrittenDicts) {
    alreadyWrittenDicts = new std::set<Dict*>;
    deleteSet = true;
  }

  if (alreadyWrittenDicts->find(dict) != alreadyWrittenDicts->end()) {
    error(errSyntaxWarning, -1, "PDFDoc::writeDictionnary: Found recursive dicts");
    if (deleteSet) delete alreadyWrittenDicts;
    return;
  } else {
    alreadyWrittenDicts->insert(dict);
  }

  outStr->printf("<<");
  for (int i=0; i<dict->getLength(); i++) {
    GooString keyName(dict->getKey(i));
    GooString *keyNameToPrint = keyName.sanitizedName(false /* non ps mode */);
    outStr->printf("/%s ", keyNameToPrint->c_str());
    delete keyNameToPrint;
    Object obj1 = dict->getValNF(i).copy();
    writeObject(&obj1, outStr, xRef, numOffset, fileKey, encAlgorithm, keyLength, objNum, objGen, alreadyWrittenDicts);
  }
  outStr->printf(">> ");

  if (deleteSet) {
    delete alreadyWrittenDicts;
  }
}