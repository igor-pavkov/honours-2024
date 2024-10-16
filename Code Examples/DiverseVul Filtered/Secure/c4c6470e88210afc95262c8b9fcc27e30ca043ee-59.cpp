int do_nothing(int8_t filter, char cmode) {
  if (cmode == 'c') {
    return (filter == BLOSC_NOFILTER);
  } else {
    // TRUNC_PREC do not have to be applied during decompression
    return ((filter == BLOSC_NOFILTER) || (filter == BLOSC_TRUNC_PREC));
  }
}