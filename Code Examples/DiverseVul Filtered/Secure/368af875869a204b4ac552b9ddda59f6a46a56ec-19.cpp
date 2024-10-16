inline const string* const* TensorProtoData<tstring>(const TensorProto& t) {
  static_assert(SaveTypeTraits<tstring>::supported,
                "Specified type tstring not supported for Restore");
  return t.string_val().data();
}