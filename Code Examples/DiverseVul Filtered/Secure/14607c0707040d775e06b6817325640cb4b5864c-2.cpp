void DoHistogram(OpKernelContext* ctx, const Tensor* labels_indices,
                 int num_indices, int batch_size,
                 std::vector<int>* labels_lengths) {
  const T* h_in = labels_indices->flat<T>().data();
  for (int i = 0; i < num_indices; i++) {
    const T& key = h_in[i * 2];
    (*labels_lengths)[key]++;
  }
}