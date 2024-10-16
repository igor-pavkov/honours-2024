int PDFDoc::saveWithoutChangesAs(const GooString *name) {
  FILE *f;
  OutStream *outStr;
  int res;

  if (!(f = openFile(name->c_str(), "wb"))) {
    error(errIO, -1, "Couldn't open file '{0:t}'", name);
    return errOpenFile;
  }
  
  outStr = new FileOutStream(f,0);
  res = saveWithoutChangesAs(outStr);
  delete outStr;

  fclose(f);

  return res;
}