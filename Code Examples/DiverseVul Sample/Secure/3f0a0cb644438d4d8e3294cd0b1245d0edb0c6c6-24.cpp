PREFIX(updatePosition)(const ENCODING *enc, const char *ptr, const char *end,
                       POSITION *pos) {
  while (HAS_CHAR(enc, ptr, end)) {
    switch (BYTE_TYPE(enc, ptr)) {
#  define LEAD_CASE(n)                                                         \
  case BT_LEAD##n:                                                             \
    ptr += n;                                                                  \
    pos->columnNumber++;                                                       \
    break;
      LEAD_CASE(2)
      LEAD_CASE(3)
      LEAD_CASE(4)
#  undef LEAD_CASE
    case BT_LF:
      pos->columnNumber = 0;
      pos->lineNumber++;
      ptr += MINBPC(enc);
      break;
    case BT_CR:
      pos->lineNumber++;
      ptr += MINBPC(enc);
      if (HAS_CHAR(enc, ptr, end) && BYTE_TYPE(enc, ptr) == BT_LF)
        ptr += MINBPC(enc);
      pos->columnNumber = 0;
      break;
    default:
      ptr += MINBPC(enc);
      pos->columnNumber++;
      break;
    }
  }
}