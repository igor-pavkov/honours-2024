long long PDFDoc::strToLongLong(const char *s) {
  long long x, d;
  const char *p;

  x = 0;
  for (p = s; *p && isdigit(*p & 0xff); ++p) {
    d = *p - '0';
    if (x > (LLONG_MAX - d) / 10) {
      break;
    }
    x = 10 * x + d;
  }
  return x;
}