  static const qint32* Begin(const TensorProto& proto) {
    return reinterpret_cast<const qint32*>(proto.int_val().data());
  }