  static TensorBuffer* Decode(Allocator* a, const Source& in, int64_t n) {
    auto* buf = new Buffer<ResourceHandle>(a, n);
    ResourceHandle* ps = buf->template base<ResourceHandle>();
    if (ps == nullptr ||
        !DecodeResourceHandleList(port::NewStringListDecoder(in), ps, n)) {
      buf->Unref();
      return nullptr;
    }
    return buf;
  }