  static void Encode(TensorBuffer* in, int64_t n, Destination* out) {
    EncodeResourceHandleList(in->base<const ResourceHandle>(), n,
                             port::NewStringListEncoder(out));
  }