void jslPrintTokenLineMarker(vcbprintf_callback user_callback, void *user_data, size_t tokenPos, char *prefix) {
  size_t line = 1,col = 1;
  jsvGetLineAndCol(lex->sourceVar, tokenPos, &line, &col);
  size_t startOfLine = jsvGetIndexFromLineAndCol(lex->sourceVar, line, 1);
  size_t lineLength = jsvGetCharsOnLine(lex->sourceVar, line);
  size_t prefixLength = 0;

  if (prefix) {
    user_callback(prefix, user_data);
    prefixLength = strlen(prefix);
  }

  if (lineLength>60 && tokenPos-startOfLine>30) {
    cbprintf(user_callback, user_data, "...");
    size_t skipChars = tokenPos-30 - startOfLine;
    startOfLine += 3+skipChars;
    if (skipChars<=col)
      col -= skipChars;
    else
      col = 0;
    lineLength -= skipChars;
  }

  // print the string until the end of the line, or 60 chars (whichever is less)
  int chars = 0;
  JsvStringIterator it;
  jsvStringIteratorNew(&it, lex->sourceVar, startOfLine);
  unsigned char lastch = 0;
  while (jsvStringIteratorHasChar(&it) && chars<60) {
    unsigned char ch = (unsigned char)jsvStringIteratorGetChar(&it);
    if (ch == '\n') break;
    if (jslNeedSpaceBetween(lastch, ch)) {
      col++;
      user_callback(" ", user_data);
    }
    char buf[32];
    jslFunctionCharAsString(ch, buf, sizeof(buf));
    size_t len = strlen(buf);
    col += len-1;
    user_callback(buf, user_data);
    chars++;
    lastch = ch;
    jsvStringIteratorNext(&it);
  }
  jsvStringIteratorFree(&it);

  if (lineLength > 60)
    user_callback("...", user_data);
  user_callback("\n", user_data);
  col += prefixLength;
  while (col-- > 1) user_callback(" ", user_data);
  user_callback("^\n", user_data);
}