  static protobuf::RepeatedPtrField<ResourceHandleProto>::const_iterator Begin(
      const TensorProto& proto) {
    return proto.resource_handle_val().begin();
  }