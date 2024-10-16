ALWAYS_INLINE void MulAdd3Way(const Packet a1, const Packet a2, const Packet a3,
                              const bfloat16** binp1, const bfloat16** binp2,
                              const bfloat16** binp3, float** out) {
  auto inp1 = reinterpret_cast<const float*>(*binp1);
  auto inp2 = reinterpret_cast<const float*>(*binp2);
  auto inp3 = reinterpret_cast<const float*>(*binp3);
  auto c1 = LOAD(*out);
  auto c2 = LOAD(*out + kNumOperands);
  const auto b1 = LOAD(inp1);
  EXPAND_BFLOAT_L(b1, b1_0);
  EXPAND_BFLOAT_U(b1, b1_1);
  *binp1 += 2 * kNumOperands;
  const auto b2 = LOAD(inp2);
  EXPAND_BFLOAT_L(b2, b2_0);
  EXPAND_BFLOAT_U(b2, b2_1);
  *binp2 += 2 * kNumOperands;
  const auto b3 = LOAD(inp3);
  EXPAND_BFLOAT_L(b3, b3_0);
  EXPAND_BFLOAT_U(b3, b3_1);
  *binp3 += 2 * kNumOperands;
  FMA(a1, b1_0, c1, c1);
  FMA(a1, b1_1, c2, c2);
  FMA(a2, b2_0, c1, c1);
  FMA(a2, b2_1, c2, c2);
  FMA(a3, b3_0, c1, c1);
  FMA(a3, b3_1, c2, c2);
  STORE(*out, c1);
  STORE(*out + kNumOperands, c2);
  *out += 2 * kNumOperands;
}