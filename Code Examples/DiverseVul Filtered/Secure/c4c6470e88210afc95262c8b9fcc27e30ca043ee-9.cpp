static void flags_to_filters(const uint8_t flags, uint8_t* filters) {
  /* Initialize the filter pipeline */
  memset(filters, 0, BLOSC2_MAX_FILTERS);
  /* Fill the filter pipeline */
  if (flags & BLOSC_DOSHUFFLE)
    filters[BLOSC2_MAX_FILTERS - 1] = BLOSC_SHUFFLE;
  if (flags & BLOSC_DOBITSHUFFLE)
    filters[BLOSC2_MAX_FILTERS - 1] = BLOSC_BITSHUFFLE;
  if (flags & BLOSC_DODELTA)
    filters[BLOSC2_MAX_FILTERS - 2] = BLOSC_DELTA;
}