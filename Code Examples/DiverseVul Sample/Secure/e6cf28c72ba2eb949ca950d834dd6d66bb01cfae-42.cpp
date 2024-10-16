ALWAYS_INLINE void LoadSingleScalar(const bfloat16** data, Packet* l) {
  auto tmp = ConvertBfloat16ToFloat(*data);
  *l = Eigen::internal::pset1<Packet>(tmp);
  ++*data;
}