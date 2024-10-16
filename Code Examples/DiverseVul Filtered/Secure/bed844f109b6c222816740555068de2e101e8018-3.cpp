void jslFunctionCharAsString(unsigned char ch, char *str, size_t len) {
  if (ch >= LEX_TOKEN_START) {
    jslTokenAsString(ch, str, len);
  } else {
    str[0] = (char)ch;
    str[1] = 0;
  }
}