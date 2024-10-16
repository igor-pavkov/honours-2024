Tensor Tensor::Slice(int64_t start, int64_t limit) const {
  CHECK_GE(dims(), 1);
  CHECK_LE(0, start);
  CHECK_LE(start, limit);
  int64_t dim0_size = shape_.dim_size(0);
  CHECK_LE(limit, dim0_size);
  if ((start == 0) && (limit == dim0_size)) {
    return *this;
  }
  Tensor ret;
  ret.shape_ = shape_;
  ret.set_dtype(dtype());
  ret.buf_ = nullptr;
  if (dim0_size > 0) {
    const int64_t elems_per_dim0 = NumElements() / dim0_size;
    const int64_t delta = start * elems_per_dim0;
    dim0_size = limit - start;
    ret.shape_.set_dim(0, dim0_size);
    const int64_t num_elems = dim0_size * elems_per_dim0;
    if (buf_) {
      DataType dt = dtype();
      CASES(dt, ret.buf_ = new SubBuffer<T>(buf_, delta, num_elems));
    }
  }
  return ret;
}