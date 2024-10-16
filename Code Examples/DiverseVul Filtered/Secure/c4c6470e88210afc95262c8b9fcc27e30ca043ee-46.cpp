void build_filters(const int doshuffle, const int delta,
                   const size_t typesize, uint8_t* filters) {

  /* Fill the end part of the filter pipeline */
  if ((doshuffle == BLOSC_SHUFFLE) && (typesize > 1))
    filters[BLOSC2_MAX_FILTERS - 1] = BLOSC_SHUFFLE;
  if (doshuffle == BLOSC_BITSHUFFLE)
    filters[BLOSC2_MAX_FILTERS - 1] = BLOSC_BITSHUFFLE;
  if (delta)
    filters[BLOSC2_MAX_FILTERS - 2] = BLOSC_DELTA;
}