ALWAYS_INLINE void TwoMulAdd3Way(const Packet a1, const Packet a2,
                                 const Packet a3, const float** inp1,
                                 const float** inp2, const float** inp3,
                                 float** out) {
  auto c1 = LOAD(*out);
  const auto b1 = LOAD(*inp1);
  const auto b2 = LOAD(*inp2);
  const auto b3 = LOAD(*inp3);

  auto c2 = LOAD(*out + kNumOperands);
  const auto b4 = LOAD(*inp1 + kNumOperands);
  const auto b5 = LOAD(*inp2 + kNumOperands);
  const auto b6 = LOAD(*inp3 + kNumOperands);

  FMA(a1, b1, c1, c1);
  FMA(a1, b4, c2, c2);
  FMA(a2, b2, c1, c1);
  FMA(a2, b5, c2, c2);
  FMA(a3, b3, c1, c1);
  FMA(a3, b6, c2, c2);
  STORE(*out, c1);
  STORE(*out + kNumOperands, c2);
  *out += 2 * kNumOperands;
  *inp1 += 2 * kNumOperands;
  *inp2 += 2 * kNumOperands;
  *inp3 += 2 * kNumOperands;
}