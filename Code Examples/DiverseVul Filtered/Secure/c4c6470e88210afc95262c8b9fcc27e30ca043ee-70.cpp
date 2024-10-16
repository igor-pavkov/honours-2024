int blosc_decompress(const void* src, void* dest, size_t destsize) {
  return blosc2_decompress(src, INT32_MAX, dest, (int32_t)destsize);
}