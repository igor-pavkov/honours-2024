  static size_t NumElements(const TensorProto& proto) {
    return proto.uint64_val().size();
  }