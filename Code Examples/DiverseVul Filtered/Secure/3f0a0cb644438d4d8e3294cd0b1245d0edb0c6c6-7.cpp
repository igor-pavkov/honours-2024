PREFIX(contentTok)(const ENCODING *enc, const char *ptr, const char *end,
                   const char **nextTokPtr) {
  if (ptr >= end)
    return XML_TOK_NONE;
  if (MINBPC(enc) > 1) {
    size_t n = end - ptr;
    if (n & (MINBPC(enc) - 1)) {
      n &= ~(MINBPC(enc) - 1);
      if (n == 0)
        return XML_TOK_PARTIAL;
      end = ptr + n;
    }
  }
  switch (BYTE_TYPE(enc, ptr)) {
  case BT_LT:
    return PREFIX(scanLt)(enc, ptr + MINBPC(enc), end, nextTokPtr);
  case BT_AMP:
    return PREFIX(scanRef)(enc, ptr + MINBPC(enc), end, nextTokPtr);
  case BT_CR:
    ptr += MINBPC(enc);
    if (! HAS_CHAR(enc, ptr, end))
      return XML_TOK_TRAILING_CR;
    if (BYTE_TYPE(enc, ptr) == BT_LF)
      ptr += MINBPC(enc);
    *nextTokPtr = ptr;
    return XML_TOK_DATA_NEWLINE;
  case BT_LF:
    *nextTokPtr = ptr + MINBPC(enc);
    return XML_TOK_DATA_NEWLINE;
  case BT_RSQB:
    ptr += MINBPC(enc);
    if (! HAS_CHAR(enc, ptr, end))
      return XML_TOK_TRAILING_RSQB;
    if (! CHAR_MATCHES(enc, ptr, ASCII_RSQB))
      break;
    ptr += MINBPC(enc);
    if (! HAS_CHAR(enc, ptr, end))
      return XML_TOK_TRAILING_RSQB;
    if (! CHAR_MATCHES(enc, ptr, ASCII_GT)) {
      ptr -= MINBPC(enc);
      break;
    }
    *nextTokPtr = ptr;
    return XML_TOK_INVALID;
    INVALID_CASES(ptr, nextTokPtr)
  default:
    ptr += MINBPC(enc);
    break;
  }
  while (HAS_CHAR(enc, ptr, end)) {
    switch (BYTE_TYPE(enc, ptr)) {
#  define LEAD_CASE(n)                                                         \
  case BT_LEAD##n:                                                             \
    if (end - ptr < n || IS_INVALID_CHAR(enc, ptr, n)) {                       \
      *nextTokPtr = ptr;                                                       \
      return XML_TOK_DATA_CHARS;                                               \
    }                                                                          \
    ptr += n;                                                                  \
    break;
      LEAD_CASE(2)
      LEAD_CASE(3)
      LEAD_CASE(4)
#  undef LEAD_CASE
    case BT_RSQB:
      if (HAS_CHARS(enc, ptr, end, 2)) {
        if (! CHAR_MATCHES(enc, ptr + MINBPC(enc), ASCII_RSQB)) {
          ptr += MINBPC(enc);
          break;
        }
        if (HAS_CHARS(enc, ptr, end, 3)) {
          if (! CHAR_MATCHES(enc, ptr + 2 * MINBPC(enc), ASCII_GT)) {
            ptr += MINBPC(enc);
            break;
          }
          *nextTokPtr = ptr + 2 * MINBPC(enc);
          return XML_TOK_INVALID;
        }
      }
      /* fall through */
    case BT_AMP:
    case BT_LT:
    case BT_NONXML:
    case BT_MALFORM:
    case BT_TRAIL:
    case BT_CR:
    case BT_LF:
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