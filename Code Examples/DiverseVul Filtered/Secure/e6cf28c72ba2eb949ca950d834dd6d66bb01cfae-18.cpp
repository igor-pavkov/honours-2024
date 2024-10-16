ALWAYS_INLINE void LoadTwoScalars(const bfloat16** data, Packet* l1,
                                  Packet* l2) {
  if (kNumOperands >= 2) {
    auto tmp = ConvertTwoBfloat16ToFloat(*data);
    *l1 = Eigen::internal::pbroadcast_first<Packet>(tmp);
    *l2 = Eigen::internal::pbroadcast_second<Packet>(tmp);
    *data += 2;
  } else {
    LoadSingleScalar(data, l1);
    LoadSingleScalar(data, l2);
  }
}