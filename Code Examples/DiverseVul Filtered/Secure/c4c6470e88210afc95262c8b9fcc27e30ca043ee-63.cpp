int last_filter(const uint8_t* filters, char cmode) {
  int last_index = -1;
  for (int i = BLOSC2_MAX_FILTERS - 1; i >= 0; i--) {
    if (!do_nothing(filters[i], cmode))  {
      last_index = i;
    }
  }
  return last_index;
}