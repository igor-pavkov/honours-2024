void FoFiTrueType::convertToCIDType0(char *psName, int *cidMap, int nCIDs,
				     FoFiOutputFunc outputFunc,
				     void *outputStream) {
  char *start;
  int length;
  FoFiType1C *ff;

  if (!getCFFBlock(&start, &length)) {
    return;
  }
  if (!(ff = FoFiType1C::make(start, length))) {
    return;
  }
  ff->convertToCIDType0(psName, cidMap, nCIDs, outputFunc, outputStream);
  delete ff;
}