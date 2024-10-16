static uint8_t get_filter_flags(const uint8_t header_flags,
                                const int32_t typesize) {
  uint8_t flags = 0;

  if ((header_flags & BLOSC_DOSHUFFLE) && (typesize > 1)) {
    flags |= BLOSC_DOSHUFFLE;
  }
  if (header_flags & BLOSC_DOBITSHUFFLE) {
    flags |= BLOSC_DOBITSHUFFLE;
  }
  if (header_flags & BLOSC_DODELTA) {
    flags |= BLOSC_DODELTA;
  }
  if (header_flags & BLOSC_MEMCPYED) {
    flags |= BLOSC_MEMCPYED;
  }
  return flags;
}