inline void Fill(const Eigen::half* data, size_t n, TensorProto* t) {
  typename protobuf::RepeatedField<int32>* val = t->mutable_half_val();
  val->Resize(n, 0);
  for (size_t i = 0; i < n; ++i) {
    val->Set(i, Eigen::numext::bit_cast<uint16>(data[i]));
  }
}