void Tensor::FillDescription(TensorDescription* description) const {
  description->set_dtype(dtype());
  shape().AsProto(description->mutable_shape());
  if (buf_ != nullptr && buf_->data() != nullptr) {
    buf_->FillAllocationDescription(
        description->mutable_allocation_description());
  }
}