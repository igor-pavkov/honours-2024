static uint8_t filters_to_flags(const uint8_t* filters) {
  uint8_t flags = 0;

  for (int i = 0; i < BLOSC2_MAX_FILTERS; i++) {
    switch (filters[i]) {
      case BLOSC_SHUFFLE:
        flags |= BLOSC_DOSHUFFLE;
        break;
      case BLOSC_BITSHUFFLE:
        flags |= BLOSC_DOBITSHUFFLE;
        break;
      case BLOSC_DELTA:
        flags |= BLOSC_DODELTA;
        break;
      default :
        break;
    }
  }
  return flags;
}