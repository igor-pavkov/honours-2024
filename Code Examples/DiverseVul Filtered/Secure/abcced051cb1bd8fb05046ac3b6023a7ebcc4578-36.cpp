  static size_t NumElements(const TensorProto& proto) {
    return proto.dcomplex_val().size() / 2;
  }