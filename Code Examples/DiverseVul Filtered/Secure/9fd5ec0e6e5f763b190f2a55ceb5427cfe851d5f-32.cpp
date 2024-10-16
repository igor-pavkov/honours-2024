void PDFDoc::checkHeader() {
  char hdrBuf[headerSearchSize+1];
  char *p;
  char *tokptr;
  int i;
  int bytesRead;

  pdfMajorVersion = 0;
  pdfMinorVersion = 0;

  // read up to headerSearchSize bytes from the beginning of the document
  for (i = 0; i < headerSearchSize; ++i) {
    const int c = str->getChar();
    if (c == EOF)
      break;
    hdrBuf[i] = c;
  }
  bytesRead = i;
  hdrBuf[bytesRead] = '\0';

  // find the start of the PDF header if it exists and parse the version
  bool headerFound = false;
  for (i = 0; i < bytesRead - 5; ++i) {
    if (!strncmp(&hdrBuf[i], "%PDF-", 5)) {
      headerFound = true;
      break;
    }
  }
  if (!headerFound) {
    error(errSyntaxWarning, -1, "May not be a PDF file (continuing anyway)");
    return;
  }
  str->moveStart(i);
  if (!(p = strtok_r(&hdrBuf[i+5], " \t\n\r", &tokptr))) {
    error(errSyntaxWarning, -1, "May not be a PDF file (continuing anyway)");
    return;
  }
  sscanf(p, "%d.%d", &pdfMajorVersion, &pdfMinorVersion);
  // We don't do the version check. Don't add it back in.
}