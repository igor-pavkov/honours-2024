ALWAYS_INLINE Packet ConvertTwoBfloat16ToFloat(const bfloat16* src) {
  return Eigen::internal::pload2bf16<Packet>(
      reinterpret_cast<const float*>(src));
}