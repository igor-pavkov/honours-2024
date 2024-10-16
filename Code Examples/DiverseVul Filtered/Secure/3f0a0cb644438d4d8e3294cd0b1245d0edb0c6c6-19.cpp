PREFIX(scanHexCharRef)(const ENCODING *enc, const char *ptr, const char *end,
                       const char **nextTokPtr) {
  if (HAS_CHAR(enc, ptr, end)) {
    switch (BYTE_TYPE(enc, ptr)) {
    case BT_DIGIT:
    case BT_HEX:
      break;
    default:
      *nextTokPtr = ptr;
      return XML_TOK_INVALID;
    }
    for (ptr += MINBPC(enc); HAS_CHAR(enc, ptr, end); ptr += MINBPC(enc)) {
      switch (BYTE_TYPE(enc, ptr)) {
      case BT_DIGIT:
      case BT_HEX:
        break;
      case BT_SEMI:
        *nextTokPtr = ptr + MINBPC(enc);
        return XML_TOK_CHAR_REF;
      default:
        *nextTokPtr = ptr;
        return XML_TOK_INVALID;
      }
    }
  }
  return XML_TOK_PARTIAL;
}