PREFIX(scanLit)(int open, const ENCODING *enc, const char *ptr, const char *end,
                const char **nextTokPtr) {
  while (HAS_CHAR(enc, ptr, end)) {
    int t = BYTE_TYPE(enc, ptr);
    switch (t) {
      INVALID_CASES(ptr, nextTokPtr)
    case BT_QUOT:
    case BT_APOS:
      ptr += MINBPC(enc);
      if (t != open)
        break;
      if (! HAS_CHAR(enc, ptr, end))
        return -XML_TOK_LITERAL;
      *nextTokPtr = ptr;
      switch (BYTE_TYPE(enc, ptr)) {
      case BT_S:
      case BT_CR:
      case BT_LF:
      case BT_GT:
      case BT_PERCNT:
      case BT_LSQB:
        return XML_TOK_LITERAL;
      default:
        return XML_TOK_INVALID;
      }
    default:
      ptr += MINBPC(enc);
      break;
    }
  }
  return XML_TOK_PARTIAL;
}