PREFIX(charRefNumber)(const ENCODING *enc, const char *ptr) {
  int result = 0;
  /* skip &# */
  UNUSED_P(enc);
  ptr += 2 * MINBPC(enc);
  if (CHAR_MATCHES(enc, ptr, ASCII_x)) {
    for (ptr += MINBPC(enc); ! CHAR_MATCHES(enc, ptr, ASCII_SEMI);
         ptr += MINBPC(enc)) {
      int c = BYTE_TO_ASCII(enc, ptr);
      switch (c) {
      case ASCII_0:
      case ASCII_1:
      case ASCII_2:
      case ASCII_3:
      case ASCII_4:
      case ASCII_5:
      case ASCII_6:
      case ASCII_7:
      case ASCII_8:
      case ASCII_9:
        result <<= 4;
        result |= (c - ASCII_0);
        break;
      case ASCII_A:
      case ASCII_B:
      case ASCII_C:
      case ASCII_D:
      case ASCII_E:
      case ASCII_F:
        result <<= 4;
        result += 10 + (c - ASCII_A);
        break;
      case ASCII_a:
      case ASCII_b:
      case ASCII_c:
      case ASCII_d:
      case ASCII_e:
      case ASCII_f:
        result <<= 4;
        result += 10 + (c - ASCII_a);
        break;
      }
      if (result >= 0x110000)
        return -1;
    }
  } else {
    for (; ! CHAR_MATCHES(enc, ptr, ASCII_SEMI); ptr += MINBPC(enc)) {
      int c = BYTE_TO_ASCII(enc, ptr);
      result *= 10;
      result += (c - ASCII_0);
      if (result >= 0x110000)
        return -1;
    }
  }
  return checkCharRefNumber(result);
}