ALWAYS_INLINE void LoadThreeScalars(const T** data, Packet* l1, Packet* l2,
                                    Packet* l3) {
  LoadTwoScalars(data, l1, l2);
  LoadSingleScalar(data, l3);
}