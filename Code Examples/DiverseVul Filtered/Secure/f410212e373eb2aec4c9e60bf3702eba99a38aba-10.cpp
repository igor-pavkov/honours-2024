  static Status Compute(OpKernelContext* context,
                        const typename TTypes<Tidx, 2>::ConstTensor& in,
                        const typename TTypes<T, 2>::ConstTensor& weights,
                        typename TTypes<T, 2>::Tensor& out,
                        const Tidx num_bins) {
    const int num_rows = out.dimension(0);
    const int num_cols = in.dimension(1);
    ThreadPool* thread_pool =
        context->device()->tensorflow_cpu_worker_threads()->workers;
    thread_pool->ParallelForWithWorkerId(
        num_rows, 8 /* cost */,
        [&](int64_t start_row, int64_t end_row, int64_t worker_id) {
          for (int64_t i = start_row; i < end_row; ++i) {
            for (int64_t j = 0; j < num_cols; ++j) {
              Tidx value = in(i, j);
              if (value < num_bins) {
                if (binary_output) {
                  out(i, value) = T(1);
                } else {
                  if (weights.size()) {
                    out(i, value) += weights(i, j);
                  } else {
                    out(i, value) += T(1);
                  }
                }
              }
            }
          }
        });
    return Status::OK();
  }