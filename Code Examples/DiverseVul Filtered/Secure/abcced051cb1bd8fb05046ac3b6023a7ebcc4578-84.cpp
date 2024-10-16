  static int64_t TotalBytes(TensorBuffer* in, int64_t n) {
    DCHECK_EQ(in->size(), sizeof(T) * n);
    return in->size();
  }