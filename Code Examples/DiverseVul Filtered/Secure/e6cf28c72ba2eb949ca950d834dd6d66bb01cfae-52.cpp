ALWAYS_INLINE void LoadSingleScalar(const float** data, Packet* l) {
  *l = Eigen::internal::pload1<Packet>(*data);
  ++(*data);
}