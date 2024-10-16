void Tensor::AsProtoField(TensorProto* proto) const {
  proto->Clear();
  shape_.AsProto(proto->mutable_tensor_shape());
  proto->set_dtype(dtype());
  if (buf_) {
    CASES(dtype(), ToProtoField<T>(*buf_, shape_.num_elements(), proto));
  }
}