  static void Encode(TensorBuffer* in, int64_t n, Destination* out) {
    port::EncodeStringList(in->base<const tstring>(), n, out);
  }