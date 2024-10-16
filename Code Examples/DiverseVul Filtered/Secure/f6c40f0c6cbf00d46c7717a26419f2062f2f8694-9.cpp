inline uint8x8_t ToUint8x8(const quint8* v0, const quint8* v1, const quint8* v2,
                           const quint8* v3, const quint8* v4, const quint8* v5,
                           const quint8* v6, const quint8* v7) {
  static const uint8x8_t ZERO_8x8 = vmov_n_u8(0);
  uint8x8_t ret = vld1_lane_u8(reinterpret_cast<const uint8*>(v0), ZERO_8x8, 0);
  ret = vld1_lane_u8(reinterpret_cast<const uint8*>(v1), ret, 1);
  ret = vld1_lane_u8(reinterpret_cast<const uint8*>(v2), ret, 2);
  ret = vld1_lane_u8(reinterpret_cast<const uint8*>(v3), ret, 3);
  ret = vld1_lane_u8(reinterpret_cast<const uint8*>(v4), ret, 4);
  ret = vld1_lane_u8(reinterpret_cast<const uint8*>(v5), ret, 5);
  ret = vld1_lane_u8(reinterpret_cast<const uint8*>(v6), ret, 6);
  ret = vld1_lane_u8(reinterpret_cast<const uint8*>(v7), ret, 7);
  return ret;
}