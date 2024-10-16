ALWAYS_INLINE void LoadFourScalars(const float** data, Packet* l1, Packet* l2,
                                   Packet* l3, Packet* l4) {
  LoadTwoScalars(data, l1, l2);
  LoadTwoScalars(data, l3, l4);
}