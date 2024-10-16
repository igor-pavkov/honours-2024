ALWAYS_INLINE void LoadSixScalars(const T** data, Packet* l1, Packet* l2,
                                  Packet* l3, Packet* l4, Packet* l5,
                                  Packet* l6) {
  LoadFourScalars(data, l1, l2, l3, l4);
  LoadTwoScalars(data, l5, l6);
}