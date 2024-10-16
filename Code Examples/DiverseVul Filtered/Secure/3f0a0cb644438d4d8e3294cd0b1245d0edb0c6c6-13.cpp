PREFIX(predefinedEntityName)(const ENCODING *enc, const char *ptr,
                             const char *end) {
  UNUSED_P(enc);
  switch ((end - ptr) / MINBPC(enc)) {
  case 2:
    if (CHAR_MATCHES(enc, ptr + MINBPC(enc), ASCII_t)) {
      switch (BYTE_TO_ASCII(enc, ptr)) {
      case ASCII_l:
        return ASCII_LT;
      case ASCII_g:
        return ASCII_GT;
      }
    }
    break;
  case 3:
    if (CHAR_MATCHES(enc, ptr, ASCII_a)) {
      ptr += MINBPC(enc);
      if (CHAR_MATCHES(enc, ptr, ASCII_m)) {
        ptr += MINBPC(enc);
        if (CHAR_MATCHES(enc, ptr, ASCII_p))
          return ASCII_AMP;
      }
    }
    break;
  case 4:
    switch (BYTE_TO_ASCII(enc, ptr)) {
    case ASCII_q:
      ptr += MINBPC(enc);
      if (CHAR_MATCHES(enc, ptr, ASCII_u)) {
        ptr += MINBPC(enc);
        if (CHAR_MATCHES(enc, ptr, ASCII_o)) {
          ptr += MINBPC(enc);
          if (CHAR_MATCHES(enc, ptr, ASCII_t))
            return ASCII_QUOT;
        }
      }
      break;
    case ASCII_a:
      ptr += MINBPC(enc);
      if (CHAR_MATCHES(enc, ptr, ASCII_p)) {
        ptr += MINBPC(enc);
        if (CHAR_MATCHES(enc, ptr, ASCII_o)) {
          ptr += MINBPC(enc);
          if (CHAR_MATCHES(enc, ptr, ASCII_s))
            return ASCII_APOS;
        }
      }
      break;
    }
  }
  return 0;
}