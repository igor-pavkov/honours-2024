ALWAYS_INLINE void MulAdd(const Packet a, const float** inp, float** out) {
  const auto b = LOAD(*inp);
  *inp += kNumOperands;
  auto c = LOAD(*out);
  FMA(a, b, c, c);
  STORE(*out, c);
  *out += kNumOperands;
}