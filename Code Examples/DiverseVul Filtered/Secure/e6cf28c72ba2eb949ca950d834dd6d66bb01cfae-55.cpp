ALWAYS_INLINE void MulAdd3Way(const Packet a1, const Packet a2, const Packet a3,
                              const float** inp1, const float** inp2,
                              const float** inp3, float** out) {
  auto c = LOAD(*out);
  const auto b1 = LOAD(*inp1);
  *inp1 += kNumOperands;
  const auto b2 = LOAD(*inp2);
  *inp2 += kNumOperands;
  const auto b3 = LOAD(*inp3);
  *inp3 += kNumOperands;
  FMA(a1, b1, c, c);
  FMA(a2, b2, c, c);
  FMA(a3, b3, c, c);
  STORE(*out, c);
  *out += kNumOperands;
}