inline void Fill(const tstring* data, size_t n, TensorProto* t) {
  typename protobuf::RepeatedPtrField<string> copy(data, data + n);
  t->mutable_string_val()->Swap(&copy);
}