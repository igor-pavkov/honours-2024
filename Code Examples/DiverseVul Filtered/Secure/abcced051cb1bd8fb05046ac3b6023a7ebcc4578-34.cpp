  static TensorBuffer* Decode(Allocator* a, const Source& in, int64_t n) {
    Buffer<tstring>* buf = new Buffer<tstring>(a, n);
    tstring* strings = buf->template base<tstring>();
    if (strings == nullptr || !port::DecodeStringList(in, strings, n)) {
      buf->Unref();
      return nullptr;
    }
    return buf;
  }