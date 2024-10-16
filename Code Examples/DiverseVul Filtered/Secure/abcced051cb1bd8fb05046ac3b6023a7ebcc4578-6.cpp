  static void Fill(const int64_t* data, size_t n, TensorProto* proto) {
    protobuf::RepeatedField<protobuf_int64> copy(data, data + n);
    proto->mutable_int64_val()->Swap(&copy);
  }