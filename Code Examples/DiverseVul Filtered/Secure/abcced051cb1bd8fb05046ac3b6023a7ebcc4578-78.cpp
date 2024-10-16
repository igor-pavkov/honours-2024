void* Tensor::data() const {
  if (buf_ == nullptr) return nullptr;  // Don't die for empty tensors
  return static_cast<void*>(buf_->data());
}