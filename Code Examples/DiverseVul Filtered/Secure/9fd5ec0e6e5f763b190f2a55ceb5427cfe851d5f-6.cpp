int PDFDoc::saveWithoutChangesAs(OutStream *outStr) {
  int c;

  if (file && file->modificationTimeChangedSinceOpen())
    return errFileChangedSinceOpen;
  
  BaseStream *copyStr = str->copy();
  copyStr->reset();
  while ((c = copyStr->getChar()) != EOF) {
    outStr->put(c);
  }
  copyStr->close();
  delete copyStr;

  return errNone;
}