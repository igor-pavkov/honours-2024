void PDFDoc::writeXRefTableTrailer(Goffset uxrefOffset, XRef *uxref, bool writeAllEntries,
                                   int uxrefSize, OutStream* outStr, bool incrUpdate)
{
  const char *fileNameA = fileName ? fileName->c_str() : nullptr;
  // file size (doesn't include the trailer)
  unsigned int fileSize = 0;
  int c;
  str->reset();
  while ((c = str->getChar()) != EOF) {
    fileSize++;
  }
  str->close();
  Ref ref;
  ref.num = getXRef()->getRootNum();
  ref.gen = getXRef()->getRootGen();
  Object trailerDict = createTrailerDict(uxrefSize, incrUpdate, getStartXRef(), &ref,
                                         getXRef(), fileNameA, fileSize);
  writeXRefTableTrailer(std::move(trailerDict), uxref, writeAllEntries, uxrefOffset, outStr, getXRef());
}