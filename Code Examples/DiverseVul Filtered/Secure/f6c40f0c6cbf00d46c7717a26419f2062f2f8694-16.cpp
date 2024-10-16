inline int16x8_t ToInt16x8(const int16* v0, const int16* v1, const int16* v2,
                           const int16* v3, const int16* v4, const int16* v5,
                           const int16* v6, const int16* v7) {
  static const int16x8_t ZERO_16x8 = vmovq_n_s16(0);
  int16x8_t ret = vld1q_lane_s16(v0, ZERO_16x8, 0);
  ret = vld1q_lane_s16(v1, ret, 1);
  ret = vld1q_lane_s16(v2, ret, 2);
  ret = vld1q_lane_s16(v3, ret, 3);
  ret = vld1q_lane_s16(v4, ret, 4);
  ret = vld1q_lane_s16(v5, ret, 5);
  ret = vld1q_lane_s16(v6, ret, 6);
  ret = vld1q_lane_s16(v7, ret, 7);
  return ret;
}