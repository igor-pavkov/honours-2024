  static TensorBuffer* Decode(Allocator* a, const Source& in, int64_t n) {
    auto* buf = new Buffer<Variant>(a, n);
    Variant* ps = buf->template base<Variant>();
    if (ps == nullptr ||
        !DecodeVariantList(port::NewStringListDecoder(in), ps, n)) {
      buf->Unref();
      return nullptr;
    }
    return buf;
  }