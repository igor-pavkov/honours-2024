inline protobuf::RepeatedField<int32>* MutableTensorProtoData<Eigen::half>(
    TensorProto* t) {
  return t->mutable_half_val();
}