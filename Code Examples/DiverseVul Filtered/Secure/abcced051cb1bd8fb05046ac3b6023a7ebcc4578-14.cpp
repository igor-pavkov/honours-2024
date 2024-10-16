  static void Fill(const Eigen::half* data, size_t n, TensorProto* proto) {
    proto->mutable_half_val()->Reserve(n);
    for (size_t i = 0; i < n; ++i) {
      proto->mutable_half_val()->AddAlreadyReserved(
          Eigen::numext::bit_cast<uint16>(data[i]));
    }
  }