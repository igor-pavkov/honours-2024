ALWAYS_INLINE void LoadFourScalars(const bfloat16** data, Packet* l1,
                                   Packet* l2, Packet* l3, Packet* l4) {
  if (kNumOperands >= 4) {
    auto tmp = ConvertFourBfloat16ToFloat(*data);
    *l1 = Eigen::internal::pbroadcast_first<Packet>(tmp);
    *l2 = Eigen::internal::pbroadcast_second<Packet>(tmp);
    *l3 = Eigen::internal::pbroadcast_third<Packet>(tmp);
    *l4 = Eigen::internal::pbroadcast_fourth<Packet>(tmp);
    *data += 4;
  } else {
    LoadTwoScalars(data, l1, l2);
    LoadTwoScalars(data, l3, l4);
  }
}