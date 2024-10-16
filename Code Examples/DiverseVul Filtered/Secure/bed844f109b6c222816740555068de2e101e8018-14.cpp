void jslPrintPosition(vcbprintf_callback user_callback, void *user_data, size_t tokenPos) {
  size_t line,col;
  jsvGetLineAndCol(lex->sourceVar, tokenPos, &line, &col);
  if (lex->lineNumberOffset)
    line += (size_t)lex->lineNumberOffset - 1;
  cbprintf(user_callback, user_data, "line %d col %d\n", line, col);
}