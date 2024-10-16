ALWAYS_INLINE void LoadTwoScalars(const float** data, Packet* l1, Packet* l2) {
  LoadSingleScalar(data, l1);
  LoadSingleScalar(data, l2);
}