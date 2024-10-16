  static int64_t TotalBytes(TensorBuffer* in, int n) {
    int64_t tot = in->size();
    DCHECK_EQ(tot, sizeof(tstring) * n);
    const tstring* p = in->base<const tstring>();
    for (int i = 0; i < n; ++i, ++p) tot += p->size();
    return tot;
  }