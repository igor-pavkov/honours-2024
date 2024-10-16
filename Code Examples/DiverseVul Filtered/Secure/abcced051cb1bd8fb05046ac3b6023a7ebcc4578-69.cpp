  static void Encode(TensorBuffer* in, int64_t n, Destination* out) {
    DCHECK_EQ(in->size(), sizeof(T) * n);
    port::AssignRefCounted(StringPiece(in->base<const char>(), in->size()), in,
                           out);
  }