inline protobuf::RepeatedPtrField<string>* MutableTensorProtoData<tstring>(
    TensorProto* t) {
  static_assert(SaveTypeTraits<tstring>::supported,
                "Specified type tstring not supported for Save");
  return t->mutable_string_val();
}