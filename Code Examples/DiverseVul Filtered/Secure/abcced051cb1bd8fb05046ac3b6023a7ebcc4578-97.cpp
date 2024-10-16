  static size_t NumElements(const TensorProto& proto) {
    return proto.scomplex_val().size() / 2;
  }