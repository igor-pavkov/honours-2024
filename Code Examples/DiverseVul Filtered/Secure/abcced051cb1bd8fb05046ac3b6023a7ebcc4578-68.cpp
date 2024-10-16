Status Tensor::BitcastFrom(const Tensor& other, DataType dtype,
                           const TensorShape& shape) {
  int in_size = DataTypeSize(other.dtype());
  int out_size = DataTypeSize(dtype);
  if (in_size == 0) {
    return errors::InvalidArgument("other tensor has zero-sized data type");
  }
  if (out_size == 0) {
    return errors::InvalidArgument("specified output type is zero-sized");
  }
  if (shape.num_elements() * out_size !=
      other.shape().num_elements() * in_size) {
    return errors::InvalidArgument(
        "input and output shapes/data type sizes are not compatible");
  }
  shape_ = shape;
  shape_.set_data_type(dtype);
  if (buf_ != other.buf_) {
    UnrefIfNonNull(buf_);
    buf_ = other.buf_;
    RefIfNonNull(buf_);
  }
  return Status::OK();
}