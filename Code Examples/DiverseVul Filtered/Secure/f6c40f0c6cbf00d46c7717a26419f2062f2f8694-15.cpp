inline int32x2_t ToInt32x2(const qint32* v0, const qint32* v1) {
  static const int32x2_t ZERO_32x2 = vmov_n_s32(0);
  const int32x2_t ret0 =
      vld1_lane_s32(reinterpret_cast<const int32*>(v0), ZERO_32x2, 0);
  const int32x2_t ret1 =
      vld1_lane_s32(reinterpret_cast<const int32*>(v1), ret0, 1);
  return ret1;
}