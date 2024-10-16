static int _php_check_ignore(const char *charset)
{
  size_t clen = strlen(charset);
  if (clen >= 9 && strcmp("//IGNORE", charset+clen-8) == 0) {
    return 1;
  }
  if (clen >= 19 && strcmp("//IGNORE//TRANSLIT", charset+clen-18) == 0) {
    return 1;
  }
  return 0;
}