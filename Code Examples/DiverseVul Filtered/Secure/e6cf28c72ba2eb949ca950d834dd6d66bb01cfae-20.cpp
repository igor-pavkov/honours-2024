ALWAYS_INLINE void MulAdd3Way128(const Packet a1, const Packet a2,
                                 const Packet a3, const bfloat16** inp1,
                                 const bfloat16** inp2, const bfloat16** inp3,
                                 float** out) {
  for (int k = 0; k < 128 / (8 * kNumOperands); ++k) {
    TwoMulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
    TwoMulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
  }
}