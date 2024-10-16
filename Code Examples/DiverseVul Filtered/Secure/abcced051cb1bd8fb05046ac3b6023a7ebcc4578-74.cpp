size_t Tensor::AllocatedBytes() const {
  if (buf_) {
    size_t ret;
    if (buf_->GetAllocatedBytes(&ret)) {
      return ret;
    }
  }
  return TotalBytes();
}