ALWAYS_INLINE void CopyAndMayBeInterleaveBfloat16(void* bdst, const void* bsrc,
                                                  int num_elements) {
  DCHECK_GE(kNumOperands, 8);
  static const int kStep = kNumOperands * sizeof(float) / sizeof(bfloat16);
  const int num = (NUM_ELEM == -1) ? num_elements : NUM_ELEM;
  DCHECK_EQ(num, num_elements);
  const float* src = reinterpret_cast<const float*>(bsrc);
  float* dst = reinterpret_cast<float*>(bdst);
  for (int index = 0; index + kStep <= num; index += kStep) {
    auto in = LOAD(src);
    auto tmp = INTERLEAVE(in);
    STORE(dst, tmp);
    src += kNumOperands;
    dst += kNumOperands;
  }
  if (num % kStep != 0) {
    memcpy(dst, src, (num % kStep) * sizeof(bfloat16));
  }
}