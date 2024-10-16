PREFIX(scanCdataSection)(const ENCODING *enc, const char *ptr, const char *end,
                         const char **nextTokPtr) {
  static const char CDATA_LSQB[]
      = {ASCII_C, ASCII_D, ASCII_A, ASCII_T, ASCII_A, ASCII_LSQB};
  int i;
  UNUSED_P(enc);
  /* CDATA[ */
  REQUIRE_CHARS(enc, ptr, end, 6);
  for (i = 0; i < 6; i++, ptr += MINBPC(enc)) {
    if (! CHAR_MATCHES(enc, ptr, CDATA_LSQB[i])) {
      *nextTokPtr = ptr;
      return XML_TOK_INVALID;
    }
  }
  *nextTokPtr = ptr;
  return XML_TOK_CDATA_SECT_OPEN;
}