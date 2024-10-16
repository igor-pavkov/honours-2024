ALWAYS_INLINE float ConvertBfloat16ToFloat(const bfloat16* src) {
  float out = 0;
  auto tmp = reinterpret_cast<bfloat16*>(&out);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  tmp[0] = *src;
#else
  tmp[1] = *src;
#endif
  return out;
}