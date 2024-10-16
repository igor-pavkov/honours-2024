void jslInit(JsVar *var) {
  lex->sourceVar = jsvLockAgain(var);
  // reset stuff
  lex->tk = 0;
  lex->tokenStart.it.var = 0;
  lex->tokenStart.currCh = 0;
  lex->tokenLastStart = 0;
  lex->tokenl = 0;
  lex->tokenValue = 0;
  lex->lineNumberOffset = 0;
  // set up iterator
  jsvStringIteratorNew(&lex->it, lex->sourceVar, 0);
  jsvUnLock(lex->it.var); // see jslGetNextCh
  jslPreload();
}