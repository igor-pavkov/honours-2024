int *FoFiTrueType::getCIDToGIDMap(int *nCIDs) {
  char *start;
  int length;
  FoFiType1C *ff;
  int *map;

  *nCIDs = 0;
  if (!getCFFBlock(&start, &length)) {
    return NULL;
  }
  if (!(ff = FoFiType1C::make(start, length))) {
    return NULL;
  }
  map = ff->getCIDToGIDMap(nCIDs);
  delete ff;
  return map;
}