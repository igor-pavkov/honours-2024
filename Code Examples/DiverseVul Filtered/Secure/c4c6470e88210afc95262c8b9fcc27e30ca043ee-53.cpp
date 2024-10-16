int blosc_compress(int clevel, int doshuffle, size_t typesize, size_t nbytes,
                   const void* src, void* dest, size_t destsize) {
  return blosc2_compress(clevel, doshuffle, (int32_t)typesize, src, (int32_t)nbytes, dest, (int32_t)destsize);
}