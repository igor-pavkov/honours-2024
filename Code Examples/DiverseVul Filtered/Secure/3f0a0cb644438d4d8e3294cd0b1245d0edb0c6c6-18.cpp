PREFIX(nameMatchesAscii)(const ENCODING *enc, const char *ptr1,
                         const char *end1, const char *ptr2) {
  UNUSED_P(enc);
  for (; *ptr2; ptr1 += MINBPC(enc), ptr2++) {
    if (end1 - ptr1 < MINBPC(enc)) {
      /* This line cannot be executed.  The incoming data has already
       * been tokenized once, so incomplete characters like this have
       * already been eliminated from the input.  Retaining the
       * paranoia check is still valuable, however.
       */
      return 0; /* LCOV_EXCL_LINE */
    }
    if (! CHAR_MATCHES(enc, ptr1, *ptr2))
      return 0;
  }
  return ptr1 == end1;
}