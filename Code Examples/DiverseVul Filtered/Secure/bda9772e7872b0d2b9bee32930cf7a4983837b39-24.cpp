void ReinterpretCopySingle(T *dst, const U *src) {
  memcpy(dst, src, std::min(sizeof(T), sizeof(U)));
}