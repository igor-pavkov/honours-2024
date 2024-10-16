PREFIX(scanPi)(const ENCODING *enc, const char *ptr, const char *end,
               const char **nextTokPtr) {
  int tok;
  const char *target = ptr;
  REQUIRE_CHAR(enc, ptr, end);
  switch (BYTE_TYPE(enc, ptr)) {
    CHECK_NMSTRT_CASES(enc, ptr, end, nextTokPtr)
  default:
    *nextTokPtr = ptr;
    return XML_TOK_INVALID;
  }
  while (HAS_CHAR(enc, ptr, end)) {
    switch (BYTE_TYPE(enc, ptr)) {
      CHECK_NAME_CASES(enc, ptr, end, nextTokPtr)
    case BT_S:
    case BT_CR:
    case BT_LF:
      if (! PREFIX(checkPiTarget)(enc, target, ptr, &tok)) {
        *nextTokPtr = ptr;
        return XML_TOK_INVALID;
      }
      ptr += MINBPC(enc);
      while (HAS_CHAR(enc, ptr, end)) {
        switch (BYTE_TYPE(enc, ptr)) {
          INVALID_CASES(ptr, nextTokPtr)
        case BT_QUEST:
          ptr += MINBPC(enc);
          REQUIRE_CHAR(enc, ptr, end);
          if (CHAR_MATCHES(enc, ptr, ASCII_GT)) {
            *nextTokPtr = ptr + MINBPC(enc);
            return tok;
          }
          break;
        default:
          ptr += MINBPC(enc);
          break;
        }
      }
      return XML_TOK_PARTIAL;
    case BT_QUEST:
      if (! PREFIX(checkPiTarget)(enc, target, ptr, &tok)) {
        *nextTokPtr = ptr;
        return XML_TOK_INVALID;
      }
      ptr += MINBPC(enc);
      REQUIRE_CHAR(enc, ptr, end);
      if (CHAR_MATCHES(enc, ptr, ASCII_GT)) {
        *nextTokPtr = ptr + MINBPC(enc);
        return tok;
      }
      /* fall through */
    default:
      *nextTokPtr = ptr;
      return XML_TOK_INVALID;
    }
  }
  return XML_TOK_PARTIAL;
}