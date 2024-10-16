  static const uint64* Begin(const TensorProto& proto) {
    return reinterpret_cast<const uint64*>(proto.uint64_val().begin());
  }