void FoFiTrueType::convertToType1(char *psName, const char **newEncoding,
				  GBool ascii, FoFiOutputFunc outputFunc,
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
  ff->convertToType1(psName, newEncoding, ascii, outputFunc, outputStream);
  delete ff;
}