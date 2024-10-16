ALWAYS_INLINE void TwoMulAdd3Way(const Packet a1, const Packet a2,
                                 const Packet a3, const bfloat16** binp1,
                                 const bfloat16** binp2, const bfloat16** binp3,
                                 float** out) {
  auto inp1 = reinterpret_cast<const float*>(*binp1);
  auto inp2 = reinterpret_cast<const float*>(*binp2);
  auto inp3 = reinterpret_cast<const float*>(*binp3);
  auto c1 = LOAD(*out);
  auto c2 = LOAD(*out + kNumOperands);
  const auto b1 = LOAD(inp1);
  const auto b2 = LOAD(inp2);
  const auto b3 = LOAD(inp3);

  EXPAND_BFLOAT_L(b1, b1_0);
  EXPAND_BFLOAT_U(b1, b1_1);
  EXPAND_BFLOAT_L(b2, b2_0);
  EXPAND_BFLOAT_U(b2, b2_1);
  EXPAND_BFLOAT_L(b3, b3_0);
  EXPAND_BFLOAT_U(b3, b3_1);
  auto c3 = LOAD(*out + 2 * kNumOperands);
  auto c4 = LOAD(*out + 3 * kNumOperands);
  const auto b4 = LOAD(inp1 + kNumOperands);
  const auto b5 = LOAD(inp2 + kNumOperands);
  const auto b6 = LOAD(inp3 + kNumOperands);

  EXPAND_BFLOAT_L(b4, b4_0);
  EXPAND_BFLOAT_U(b4, b4_1);
  EXPAND_BFLOAT_L(b5, b5_0);
  EXPAND_BFLOAT_U(b5, b5_1);
  EXPAND_BFLOAT_L(b6, b6_0);
  EXPAND_BFLOAT_U(b6, b6_1);

  FMA(a1, b1_0, c1, c1);
  FMA(a1, b1_1, c2, c2);
  FMA(a1, b4_0, c3, c3);
  FMA(a1, b4_1, c4, c4);
  FMA(a2, b2_0, c1, c1);
  FMA(a2, b2_1, c2, c2);
  FMA(a2, b5_0, c3, c3);
  FMA(a2, b5_1, c4, c4);
  FMA(a3, b3_0, c1, c1);
  FMA(a3, b3_1, c2, c2);
  FMA(a3, b6_0, c3, c3);
  FMA(a3, b6_1, c4, c4);
  STORE(*out, c1);
  STORE(*out + kNumOperands, c2);
  STORE(*out + 2 * kNumOperands, c3);
  STORE(*out + 3 * kNumOperands, c4);
  *out += 4 * kNumOperands;
  *binp1 += 4 * kNumOperands;
  *binp2 += 4 * kNumOperands;
  *binp3 += 4 * kNumOperands;
}