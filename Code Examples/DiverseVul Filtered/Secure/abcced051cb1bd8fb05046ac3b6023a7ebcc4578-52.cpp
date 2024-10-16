Tensor::Tensor(DataType type, TensorShape shape,
               core::RefCountPtr<TensorBuffer> buf)
    : shape_(std::move(shape)), buf_(buf.release()) {
  set_dtype(type);
}