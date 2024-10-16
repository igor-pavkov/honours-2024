TensorBuffer* FromProtoField<Variant>(Allocator* a, const TensorProto& in,
                                      int64_t n) {
  CHECK_GT(n, 0);
  Buffer<Variant>* buf = new Buffer<Variant>(a, n);
  Variant* data = buf->template base<Variant>();
  if (data == nullptr) {
    buf->Unref();
    return nullptr;
  }
  const int64_t in_n = ProtoHelper<Variant>::NumElements(in);
  if (in_n <= 0) {
    std::fill_n(data, n, Variant());
  } else {
    // If tensor shape says we have n < in_n elements in the output tensor
    // then make sure to only decode the first n out of the in_n elements in the
    // in tensors. In all other cases, we decode all in_n elements of in and set
    // the remaining elements up to n to be the default Variant() value.
    const int64_t real_n = n < in_n ? n : in_n;
    for (int64_t i = 0; i < real_n; ++i) {
      data[i] = in.variant_val(i);
      if (!DecodeUnaryVariant(&data[i])) {
        LOG(ERROR) << "Could not decode variant with type_name: \""
                   << data[i].TypeName()
                   << "\".  Perhaps you forgot to register a "
                      "decoder via REGISTER_UNARY_VARIANT_DECODE_FUNCTION?";
        buf->Unref();
        return nullptr;
      }
    }
    for (int64_t i = in_n; i < n; ++i) {
      data[i] = Variant();
    }
  }
  return buf;
}