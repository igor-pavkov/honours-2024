void ReinterpretCopyArray(T (&dst)[M], const U (&src)[N],
                          size_t max_len = SIZE_MAX) {
  memcpy(dst, src, std::min(max_len, std::min(sizeof(T) * M, sizeof(U) * N)));
}