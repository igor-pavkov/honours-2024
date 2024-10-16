void ToProtoField(const TensorBuffer& in, int64_t n, TensorProto* out) {
  const T* data = in.base<const T>();
  // NOTE: T may not the same as
  // ProtoHelper<T>::FieldType::value_type.  E.g., T==int16,
  // ProtoHelper<T>::FieldType::value_type==int32.  If performance is
  // critical, we can specialize T=float and do memcpy directly.
  ProtoHelper<T>::Fill(data, n, out);
}