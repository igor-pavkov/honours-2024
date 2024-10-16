bool Tensor::RefCountIsOne() const {
  return buf_ != nullptr && buf_->RefCountIsOne() &&
         buf_->root_buffer()->RefCountIsOne() && buf_->OwnsMemory();
}