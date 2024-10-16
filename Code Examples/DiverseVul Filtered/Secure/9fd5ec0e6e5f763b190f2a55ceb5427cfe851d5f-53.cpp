bool PDFDoc::checkFooter() {
  // we look in the last 1024 chars because Adobe does the same
  char *eof = new char[1025];
  Goffset pos = str->getPos();
  str->setPos(1024, -1);
  int i, ch;
  for (i = 0; i < 1024; i++)
  {
    ch = str->getChar();
    if (ch == EOF)
      break;
    eof[i] = ch;
  }
  eof[i] = '\0';

  bool found = false;
  for (i = i - 5; i >= 0; i--) {
    if (strncmp (&eof[i], "%%EOF", 5) == 0) {
      found = true;
      break;
    }
  }
  if (!found)
  {
    error(errSyntaxError, -1, "Document has not the mandatory ending %%EOF");
    errCode = errDamaged;
    delete[] eof;
    return false;
  }
  delete[] eof;
  str->setPos(pos);
  return true;
}