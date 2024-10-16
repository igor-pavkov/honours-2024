bool jslIsIDOrReservedWord() {
  return lex->tk == LEX_ID ||
         (lex->tk >= _LEX_R_LIST_START && lex->tk <= _LEX_R_LIST_END);
}