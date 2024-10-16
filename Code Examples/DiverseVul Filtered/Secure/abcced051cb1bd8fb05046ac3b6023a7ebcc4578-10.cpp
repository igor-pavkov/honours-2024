StringPiece Tensor::tensor_data() const {
  if (buf_ == nullptr) return StringPiece();  // Don't die for empty tensors
  return StringPiece(static_cast<char*>(buf_->data()), TotalBytes());
}