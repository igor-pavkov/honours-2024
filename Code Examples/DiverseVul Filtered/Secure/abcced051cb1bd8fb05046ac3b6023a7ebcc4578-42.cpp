  static const int64_t* Begin(const TensorProto& proto) {
    return reinterpret_cast<const int64_t*>(proto.int64_val().begin());
  }