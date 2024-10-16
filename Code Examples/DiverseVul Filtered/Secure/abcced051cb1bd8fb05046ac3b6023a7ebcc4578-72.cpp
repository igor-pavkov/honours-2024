void Tensor::AsProtoTensorContent(TensorProto* proto) const {
  proto->Clear();
  proto->set_dtype(dtype());
  shape_.AsProto(proto->mutable_tensor_shape());
  if (buf_) {
    CASES(dtype(), Helper<T>::Encode(buf_, shape_.num_elements(),
                                     proto->mutable_tensor_content()));
  }
}