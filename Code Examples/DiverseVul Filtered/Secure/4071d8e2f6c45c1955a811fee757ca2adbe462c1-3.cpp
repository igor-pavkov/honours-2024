void ReverseRows(OpKernelContext* context, const Tensor& input,
                 Tensor* result) {
  auto work = [&input, result](int64 start, int64 end) {
    const int64 inner_size =
        NUM_CHANNELS > 0 ? NUM_CHANNELS : input.dim_size(2);
    const int64 middle_size = input.dim_size(1);
    const int64 row_size = inner_size * middle_size;
    DCHECK_EQ(input.dim_size(2), inner_size);

    const T* in_ptr = input.bit_casted_tensor<T, 3>().data();
    T* out_ptr = result->bit_casted_tensor<T, 3>().data();

    in_ptr += start * row_size;
    out_ptr += start * row_size;

    for (int outer_dim = start; outer_dim < end; ++outer_dim) {
      out_ptr += row_size;
      int remaining = middle_size;
      while (remaining > 0) {
        out_ptr -= inner_size;
        memcpy(out_ptr, in_ptr, inner_size * sizeof(T));
        in_ptr += inner_size;
        --remaining;
      }

      out_ptr += row_size;
    }
  };

  // Shard across outer dimension.
  const int64 N = input.dim_size(0);
  const int64 cost_per_unit = input.NumElements() / N;
  auto worker_threads = context->device()->tensorflow_cpu_worker_threads();
  Shard(worker_threads->num_threads, worker_threads->workers, N, cost_per_unit,
        std::move(work));
}