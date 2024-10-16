static ALWAYS_INLINE char jslNextCh() {
  return (char)(lex->it.ptr ? READ_FLASH_UINT8(&lex->it.ptr[lex->it.charIdx]) : 0);
}