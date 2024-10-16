int next_filter(const uint8_t* filters, int current_filter, char cmode) {
  for (int i = current_filter - 1; i >= 0; i--) {
    if (!do_nothing(filters[i], cmode)) {
      return filters[i];
    }
  }
  return BLOSC_NOFILTER;
}