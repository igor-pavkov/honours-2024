inline const int32* TensorProtoData<qint32>(const TensorProto& t) {
  static_assert(SaveTypeTraits<qint32>::supported,
                "Specified type qint32 not supported for Restore");
  return reinterpret_cast<const int32*>(t.int_val().data());
}