  static const complex64* Begin(const TensorProto& proto) {
    return reinterpret_cast<const complex64*>(proto.scomplex_val().data());
  }