  static Status Compute(OpKernelContext* context,
                        const typename TTypes<Tidx, 1>::ConstTensor& arr,
                        const typename TTypes<T, 1>::ConstTensor& weights,
                        typename TTypes<T, 1>::Tensor& output,
                        const Tidx num_bins) {
    Tensor all_nonneg_t;
    TF_RETURN_IF_ERROR(context->allocate_temp(
        DT_BOOL, TensorShape({}), &all_nonneg_t, AllocatorAttributes()));
    all_nonneg_t.scalar<bool>().device(context->eigen_cpu_device()) =
        (arr >= Tidx(0)).all();
    if (!all_nonneg_t.scalar<bool>()()) {
      return errors::InvalidArgument("Input arr must be non-negative!");
    }

    // Allocate partial output bin sums for each worker thread. Worker ids in
    // ParallelForWithWorkerId range from 0 to NumThreads() inclusive.
    ThreadPool* thread_pool =
        context->device()->tensorflow_cpu_worker_threads()->workers;
    const int64_t num_threads = thread_pool->NumThreads() + 1;
    const Tidx* arr_data = arr.data();
    const std::ptrdiff_t arr_size = arr.size();
    const T* weight_data = weights.data();
    if (weights.size() && weights.size() != arr_size) {
      return errors::InvalidArgument(
          "Input indices and weights must have the same size.");
    }
    if (num_threads == 1) {
      output.setZero();
      T* output_data = output.data();
      if (weights.size()) {
        for (int64_t i = 0; i < arr_size; i++) {
          const Tidx value = arr_data[i];
          if (value < num_bins) {
            output_data[value] += weight_data[i];
          }
        }
      } else {
        for (int64_t i = 0; i < arr_size; i++) {
          const Tidx value = arr_data[i];
          if (value < num_bins) {
            // Complex numbers don't support "++".
            output_data[value] += T(1);
          }
        }
      }
    } else {
      Tensor partial_bins_t;
      TF_RETURN_IF_ERROR(context->allocate_temp(
          DataTypeToEnum<T>::value, TensorShape({num_threads, num_bins}),
          &partial_bins_t));
      auto partial_bins = partial_bins_t.matrix<T>();
      partial_bins.setZero();
      thread_pool->ParallelForWithWorkerId(
          arr_size, 8 /* cost */,
          [&](int64_t start_ind, int64_t limit_ind, int64_t worker_id) {
            if (weights.size()) {
              for (int64_t i = start_ind; i < limit_ind; i++) {
                Tidx value = arr_data[i];
                if (value < num_bins) {
                  partial_bins(worker_id, value) += weight_data[i];
                }
              }
            } else {
              for (int64_t i = start_ind; i < limit_ind; i++) {
                Tidx value = arr_data[i];
                if (value < num_bins) {
                  // Complex numbers don't support "++".
                  partial_bins(worker_id, value) += T(1);
                }
              }
            }
          });

      // Sum the partial bins along the 0th axis.
      Eigen::array<int, 1> reduce_dim({0});
      output.device(context->eigen_cpu_device()) = partial_bins.sum(reduce_dim);
    }
    return Status::OK();
  }