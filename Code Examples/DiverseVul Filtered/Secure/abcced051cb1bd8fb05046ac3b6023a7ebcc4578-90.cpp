  static size_t NumElements(const TensorProto& proto) {
    return proto.int64_val().size();
  }