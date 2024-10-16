TensorBuffer* FromProtoField<Eigen::half>(Allocator* a, const TensorProto& in,
                                          int64_t n) {
  CHECK_GT(n, 0);
  Buffer<Eigen::half>* buf = new Buffer<Eigen::half>(a, n);
  uint16* data = buf->template base<uint16>();
  if (data == nullptr) {
    buf->Unref();
    return nullptr;
  }
  const int64_t in_n = in.half_val().size();
  auto begin = in.half_val().begin();
  if (n <= in_n) {
    std::copy_n(begin, n, data);
  } else if (in_n > 0) {
    std::copy_n(begin, in_n, data);
    const uint16 last = *(data + in_n - 1);
    std::fill_n(data + in_n, n - in_n, last);
  } else {
    std::fill_n(data, n, 0);
  }
  return buf;
}