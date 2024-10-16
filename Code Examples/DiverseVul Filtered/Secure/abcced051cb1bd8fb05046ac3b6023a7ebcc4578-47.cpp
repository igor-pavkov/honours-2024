  static void Encode(TensorBuffer* in, int64_t n, Destination* out) {
    EncodeVariantList(in->base<const Variant>(), n,
                      port::NewStringListEncoder(out));
  }