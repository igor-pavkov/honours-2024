unsigned int jslGetLineNumber() {
  size_t line;
  size_t col;
  jsvGetLineAndCol(lex->sourceVar, jsvStringIteratorGetIndex(&lex->tokenStart.it)-1, &line, &col);
  return (unsigned int)line;
}