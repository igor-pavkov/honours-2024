PREFIX(scanDecl)(const ENCODING *enc, const char *ptr, const char *end,
                 const char **nextTokPtr) {
  REQUIRE_CHAR(enc, ptr, end);
  switch (BYTE_TYPE(enc, ptr)) {
  case BT_MINUS:
    return PREFIX(scanComment)(enc, ptr + MINBPC(enc), end, nextTokPtr);
  case BT_LSQB:
    *nextTokPtr = ptr + MINBPC(enc);
    return XML_TOK_COND_SECT_OPEN;
  case BT_NMSTRT:
  case BT_HEX:
    ptr += MINBPC(enc);
    break;
  default:
    *nextTokPtr = ptr;
    return XML_TOK_INVALID;
  }
  while (HAS_CHAR(enc, ptr, end)) {
    switch (BYTE_TYPE(enc, ptr)) {
    case BT_PERCNT:
      REQUIRE_CHARS(enc, ptr, end, 2);
      /* don't allow <!ENTITY% foo "whatever"> */
      switch (BYTE_TYPE(enc, ptr + MINBPC(enc))) {
      case BT_S:
      case BT_CR:
      case BT_LF:
      case BT_PERCNT:
        *nextTokPtr = ptr;
        return XML_TOK_INVALID;
      }
      /* fall through */
    case BT_S:
    case BT_CR:
    case BT_LF:
      *nextTokPtr = ptr;
      return XML_TOK_DECL_OPEN;
    case BT_NMSTRT:
    case BT_HEX:
      ptr += MINBPC(enc);
      break;
    default:
      *nextTokPtr = ptr;
      return XML_TOK_INVALID;
    }
  }
  return XML_TOK_PARTIAL;
}