  static void Fill(const uint64* data, size_t n, TensorProto* proto) {
    protobuf::RepeatedField<protobuf_uint64> copy(data, data + n);
    proto->mutable_uint64_val()->Swap(&copy);
  }