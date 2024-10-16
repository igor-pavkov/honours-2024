  static size_t NumElements(const TensorProto& proto) {
    return proto.variant_val().size();
  }