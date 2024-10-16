Tensor::Tensor(Allocator* a, DataType type, const TensorShape& shape)
    : shape_(shape), buf_(nullptr) {
  set_dtype(type);
  CHECK_NOTNULL(a);
  if (shape_.num_elements() > 0 || a->AllocatesOpaqueHandle()) {
    CASES(type, buf_ = new Buffer<T>(a, shape.num_elements()));
  }
  if (MemoryLoggingEnabled() && buf_ != nullptr && buf_->data() != nullptr) {
    LogMemory::RecordTensorAllocation("Unknown", LogMemory::UNKNOWN_STEP_ID,
                                      *this);
  }
}