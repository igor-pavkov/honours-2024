inline void Fill(const qint32* data, size_t n, TensorProto* t) {
  const int32* p = reinterpret_cast<const int32*>(data);
  typename protobuf::RepeatedField<int32> copy(p, p + n);
  t->mutable_int_val()->Swap(&copy);
}