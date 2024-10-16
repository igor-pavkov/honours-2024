ALWAYS_INLINE void CopyAndMayBeInterleave(void* dst, const void* src,
                                          int num_elements) {
  if (std::is_same<T, float>::value || kNumOperands < 8) {
    memcpy(dst, src, num_elements * sizeof(T));
  } else if (std::is_same<T, bfloat16>::value) {
    if (num_elements == N) {
      CopyAndMayBeInterleaveBfloat16<N>(dst, src, num_elements);
    } else {
      CopyAndMayBeInterleaveBfloat16<-1>(dst, src, num_elements);
    }
  } else {
    LOG(FATAL) << "Unsupported type";
  }
}