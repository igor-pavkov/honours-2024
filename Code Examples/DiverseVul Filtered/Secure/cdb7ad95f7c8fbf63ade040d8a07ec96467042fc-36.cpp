FoFiTrueType *FoFiTrueType::make(char *fileA, int lenA, int faceIndexA) {
  FoFiTrueType *ff;

  ff = new FoFiTrueType(fileA, lenA, gFalse, faceIndexA);
  if (!ff->parsedOk) {
    delete ff;
    return NULL;
  }
  return ff;
}