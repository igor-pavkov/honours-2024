PREFIX(ignoreSectionTok)(const ENCODING *enc, const char *ptr, const char *end,
                         const char **nextTokPtr) {
  int level = 0;
  if (MINBPC(enc) > 1) {
    size_t n = end - ptr;
    if (n & (MINBPC(enc) - 1)) {
      n &= ~(MINBPC(enc) - 1);
      end = ptr + n;
    }
  }
  while (HAS_CHAR(enc, ptr, end)) {
    switch (BYTE_TYPE(enc, ptr)) {
      INVALID_CASES(ptr, nextTokPtr)
    case BT_LT:
      ptr += MINBPC(enc);
      REQUIRE_CHAR(enc, ptr, end);
      if (CHAR_MATCHES(enc, ptr, ASCII_EXCL)) {
        ptr += MINBPC(enc);
        REQUIRE_CHAR(enc, ptr, end);
        if (CHAR_MATCHES(enc, ptr, ASCII_LSQB)) {
          ++level;
          ptr += MINBPC(enc);
        }
      }
      break;
    case BT_RSQB:
      ptr += MINBPC(enc);
      REQUIRE_CHAR(enc, ptr, end);
      if (CHAR_MATCHES(enc, ptr, ASCII_RSQB)) {
        ptr += MINBPC(enc);
        REQUIRE_CHAR(enc, ptr, end);
        if (CHAR_MATCHES(enc, ptr, ASCII_GT)) {
          ptr += MINBPC(enc);
          if (level == 0) {
            *nextTokPtr = ptr;
            return XML_TOK_IGNORE_SECT;
          }
          --level;
        }
      }
      break;
    default:
      ptr += MINBPC(enc);
      break;
    }
  }
  return XML_TOK_PARTIAL;
}