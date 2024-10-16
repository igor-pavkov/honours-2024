static ALWAYS_INLINE void jslSingleChar() {
  lex->tk = (unsigned char)lex->currCh;
  jslGetNextCh();
}