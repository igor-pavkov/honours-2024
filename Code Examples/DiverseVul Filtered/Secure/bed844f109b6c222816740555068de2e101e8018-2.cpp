void jslSeekTo(size_t seekToChar) {
  if (lex->it.var) jsvLockAgain(lex->it.var); // see jslGetNextCh
  jsvStringIteratorFree(&lex->it);
  jsvStringIteratorNew(&lex->it, lex->sourceVar, seekToChar);
  jsvUnLock(lex->it.var); // see jslGetNextCh
  lex->tokenStart.it.var = 0;
  lex->tokenStart.currCh = 0;
  jslPreload();
}