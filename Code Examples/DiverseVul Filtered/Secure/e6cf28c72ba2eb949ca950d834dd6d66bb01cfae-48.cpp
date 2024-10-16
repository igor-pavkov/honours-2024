ALWAYS_INLINE Packet ConvertFourBfloat16ToFloat(const bfloat16* src) {
  return Eigen::internal::pload4bf16<Packet>(
      reinterpret_cast<const float*>(src));
}