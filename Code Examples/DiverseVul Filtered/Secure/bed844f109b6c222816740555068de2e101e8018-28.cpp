void jslSeekToP(JslCharPos *seekToChar) {
  if (lex->it.var) jsvLockAgain(lex->it.var); // see jslGetNextCh
  jsvStringIteratorFree(&lex->it);
  lex->it = jsvStringIteratorClone(&seekToChar->it);
  jsvUnLock(lex->it.var); // see jslGetNextCh
  lex->currCh = seekToChar->currCh;
  lex->tokenStart.it.var = 0;
  lex->tokenStart.currCh = 0;
  jslGetNextToken();
}