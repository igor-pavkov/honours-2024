  static TensorBuffer* Decode(Allocator* a, const Source& in, int64_t n) {
    if (in.size() != sizeof(T) * n) {
      LogUnexpectedSize(in.size(), sizeof(T) * n);
      return nullptr;
    }
    Buffer<T>* buf = new Buffer<T>(a, n);
    char* data = buf->template base<char>();
    if (data == nullptr) {
      buf->Unref();
      return nullptr;
    }
    port::CopyToArray(in, data);
    return buf;
  }