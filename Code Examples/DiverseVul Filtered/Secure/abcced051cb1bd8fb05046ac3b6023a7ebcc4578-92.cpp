  static const complex128* Begin(const TensorProto& proto) {
    return reinterpret_cast<const complex128*>(proto.dcomplex_val().data());
  }