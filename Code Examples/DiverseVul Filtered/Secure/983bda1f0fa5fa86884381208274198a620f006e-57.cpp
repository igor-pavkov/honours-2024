void CLASS dcraw_message(int code, const char *format, ...) {
  if (verbose || code!=DCRAW_VERBOSE) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
  }
}