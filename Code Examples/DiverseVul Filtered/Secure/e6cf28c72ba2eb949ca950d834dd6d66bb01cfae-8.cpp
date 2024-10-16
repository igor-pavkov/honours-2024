ALWAYS_INLINE void MulAdd3Way128(const Packet a1, const Packet a2,
                                 const Packet a3, const float** inp1,
                                 const float** inp2, const float** inp3,
                                 float** out) {
  if (kNumOperands == 8) {
    FourMulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
    FourMulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
    FourMulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
    FourMulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
  } else {
    DCHECK_LE(4 * kNumOperands, 128);
    for (int i = 0; i < 128 / (4 * kNumOperands); ++i) {
      MulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
      MulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
      MulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
      MulAdd3Way(a1, a2, a3, inp1, inp2, inp3, out);
    }
  }
}