bool Tensor::SharesBufferWith(const Tensor& b) const {
  return buf_ != nullptr && b.buf_ != nullptr &&
         buf_->root_buffer() == b.buf_->root_buffer();
}