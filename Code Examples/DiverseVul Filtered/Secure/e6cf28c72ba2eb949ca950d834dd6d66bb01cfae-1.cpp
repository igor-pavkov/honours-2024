ALWAYS_INLINE void MulAdd(const Packet a, const bfloat16** binp, float** out) {
  auto inp = reinterpret_cast<const float*>(*binp);
  const auto b = LOAD(inp);
  EXPAND_BFLOAT_L(b, b_0);
  EXPAND_BFLOAT_U(b, b_1);
  *binp += 2 * kNumOperands;
  auto c1 = LOAD(*out);
  auto c2 = LOAD(*out + kNumOperands);
  FMA(a, b_0, c1, c1);
  FMA(a, b_1, c2, c2);
  STORE(*out, c1);
  STORE(*out + kNumOperands, c2);
  *out += 2 * kNumOperands;
}