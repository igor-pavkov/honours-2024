Tensor Tensor::SubSlice(int64_t index) const {
  CHECK_GE(dims(), 1);  // Crash ok.
  CHECK_LE(0, index);   // Crash ok.
  int64_t dim0_size = shape_.dim_size(0);
  CHECK_LE(index, dim0_size);  // Crash ok.
  Tensor ret;
  ret.shape_ = shape_;
  ret.shape_.RemoveDim(0);
  ret.set_dtype(dtype());
  ret.buf_ = nullptr;
  if (dim0_size > 0) {
    const int64_t elems_per_dim0 = NumElements() / dim0_size;
    const int64_t delta = index * elems_per_dim0;
    const int64_t num_elems = elems_per_dim0;
    if (buf_) {
      DataType dt = dtype();
      CASES(dt, ret.buf_ = new SubBuffer<T>(buf_, delta, num_elems));
    }
  }
  return ret;
}