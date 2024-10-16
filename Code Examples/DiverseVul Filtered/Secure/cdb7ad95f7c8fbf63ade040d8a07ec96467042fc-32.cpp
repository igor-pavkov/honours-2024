FoFiTrueType *FoFiTrueType::load(char *fileName, int faceIndexA) {
  FoFiTrueType *ff;
  char *fileA;
  int lenA;

  if (!(fileA = FoFiBase::readFile(fileName, &lenA))) {
    return NULL;
  }
  ff = new FoFiTrueType(fileA, lenA, gTrue, faceIndexA);
  if (!ff->parsedOk) {
    delete ff;
    return NULL;
  }
  return ff;
}