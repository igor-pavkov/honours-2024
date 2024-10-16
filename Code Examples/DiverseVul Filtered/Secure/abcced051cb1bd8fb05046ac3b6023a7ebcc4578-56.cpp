bool Tensor::IsInitialized() const {
  return (buf_ != nullptr && buf_->data() != nullptr) ||
         shape_.num_elements() == 0;
}