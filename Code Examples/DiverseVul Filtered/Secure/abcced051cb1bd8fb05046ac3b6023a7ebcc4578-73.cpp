  static void Fill(const complex128* data, size_t n, TensorProto* proto) {
    const double* p = reinterpret_cast<const double*>(data);
    FieldType copy(p, p + n * 2);
    proto->mutable_dcomplex_val()->Swap(&copy);
  }