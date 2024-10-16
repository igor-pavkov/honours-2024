ALWAYS_INLINE void FourMulAdd3Way(const Packet a1, const Packet a2,
                                  const Packet a3, const float** inp1,
                                  const float** inp2, const float** inp3,
                                  float** out) {
  TwoMulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
  TwoMulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
}