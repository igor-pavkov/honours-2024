  static int64_t TotalBytes(TensorBuffer* in, int n) {
    return n * sizeof(ResourceHandle);
  }