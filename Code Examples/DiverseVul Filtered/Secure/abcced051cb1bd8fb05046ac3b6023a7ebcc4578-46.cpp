TensorBuffer* FromProtoField(Allocator* a, const TensorProto& in, int64_t n) {
  CHECK_GT(n, 0);
  Buffer<T>* buf = new Buffer<T>(a, n);
  T* data = buf->template base<T>();
  if (data == nullptr) {
    buf->Unref();
    return nullptr;
  }

  const int64_t in_n = ProtoHelper<T>::NumElements(in);
  if (in_n <= 0) {
    std::fill_n(data, n, T());
  } else {
    auto begin = ProtoHelper<T>::Begin(in);
    if (n <= in_n) {
      std::copy_n(begin, n, data);
    } else {
      std::copy_n(begin, in_n, data);
      if (std::is_trivially_copyable<T>::value) {
        const T last = *(data + in_n - 1);
        std::fill_n(data + in_n, n - in_n, last);
      } else {
        const T& last = *(data + in_n - 1);
        std::fill_n(data + in_n, n - in_n, last);
      }
    }
  }

  return buf;
}