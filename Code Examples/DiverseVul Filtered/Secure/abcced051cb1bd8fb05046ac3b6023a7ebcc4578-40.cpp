Tensor::Tensor(DataType type, const TensorShape& shape, TensorBuffer* buf)
    : shape_(shape), buf_(buf) {
  set_dtype(type);
  RefIfNonNull(buf);
}