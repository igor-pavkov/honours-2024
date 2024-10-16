PREFIX(attributeValueTok)(const ENCODING *enc, const char *ptr, const char *end,
                          const char **nextTokPtr) {
  const char *start;
  if (ptr >= end)
    return XML_TOK_NONE;
  else if (! HAS_CHAR(enc, ptr, end)) {
    /* This line cannot be executed.  The incoming data has already
     * been tokenized once, so incomplete characters like this have
     * already been eliminated from the input.  Retaining the paranoia
     * check is still valuable, however.
     */
    return XML_TOK_PARTIAL; /* LCOV_EXCL_LINE */
  }
  start = ptr;
  while (HAS_CHAR(enc, ptr, end)) {
    switch (BYTE_TYPE(enc, ptr)) {
#  define LEAD_CASE(n)                                                         \
  case BT_LEAD##n:                                                             \
    ptr += n;                                                                  \
    break;
      LEAD_CASE(2)
      LEAD_CASE(3)
      LEAD_CASE(4)
#  undef LEAD_CASE
    case BT_AMP:
      if (ptr == start)
        return PREFIX(scanRef)(enc, ptr + MINBPC(enc), end, nextTokPtr);
      *nextTokPtr = ptr;
      return XML_TOK_DATA_CHARS;
    case BT_LT:
      /* this is for inside entity references */
      *nextTokPtr = ptr;
      return XML_TOK_INVALID;
    case BT_LF:
      if (ptr == start) {
        *nextTokPtr = ptr + MINBPC(enc);
        return XML_TOK_DATA_NEWLINE;
      }
      *nextTokPtr = ptr;
      return XML_TOK_DATA_CHARS;
    case BT_CR:
      if (ptr == start) {
        ptr += MINBPC(enc);
        if (! HAS_CHAR(enc, ptr, end))
          return XML_TOK_TRAILING_CR;
        if (BYTE_TYPE(enc, ptr) == BT_LF)
          ptr += MINBPC(enc);
        *nextTokPtr = ptr;
        return XML_TOK_DATA_NEWLINE;
      }
      *nextTokPtr = ptr;
      return XML_TOK_DATA_CHARS;
    case BT_S:
      if (ptr == start) {
        *nextTokPtr = ptr + MINBPC(enc);
        return XML_TOK_ATTRIBUTE_VALUE_S;
      }
      *nextTokPtr = ptr;
      return XML_TOK_DATA_CHARS;
    default:
      ptr += MINBPC(enc);
      break;
    }
  }
  *nextTokPtr = ptr;
  return XML_TOK_DATA_CHARS;
}