  void ParallelSplitByInputData(OpKernelContext* context,
                                const InputReshapedType& input_reshaped,
                                const TensorShape& input_shape,
                                const std::vector<Tlen>& split_sizes_vec,
                                const int32_t split_dim) const {
    const T* p_data = input_reshaped.data();
    const uint32 elem_pkg = input_reshaped.dimensions().rank() == 3
                                ? input_reshaped.dimension(2)
                                : 1;
    const uint32 line_elem_num =
        (input_reshaped.dimensions().rank() >= 2 ? input_reshaped.dimension(1)
                                                 : 1) *
        elem_pkg;
    const uint32 line_num = input_reshaped.dimension(0);

    // Prepare the output matrix.
    std::vector<T*> outputs(split_sizes_vec.size());
    for (uint64 i = 0; i < split_sizes_vec.size(); ++i) {
      TensorShape output_shape(input_shape);
      output_shape.set_dim(split_dim, split_sizes_vec[i]);
      Tensor* result = nullptr;
      OP_REQUIRES_OK(context,
                     context->allocate_output(i, output_shape, &result));
      outputs[i] = static_cast<T*>(&result->flat<T>()(0));
    }

    auto sub_split_func = [&split_sizes_vec, &p_data, elem_pkg, &outputs,
                           line_elem_num](int32_t start_part,
                                          int32_t end_part) {
      int start = start_part * line_elem_num;
      int end = end_part * line_elem_num;
      uint32 times = 0;
      for (int32_t i = start; i < end;) {
        for (uint32 j = 0; j < split_sizes_vec.size(); ++j) {
          const auto copy_elem_num = split_sizes_vec[j] * elem_pkg;
          std::copy_n(p_data + i, copy_elem_num,
                      &(outputs[j][(start_part + times) * copy_elem_num]));
          i += copy_elem_num;
        }
        ++times;
      }
    };

    uint32 part_size =
        context->device()->tensorflow_cpu_worker_threads()->num_threads;
    Shard(part_size,
          context->device()->tensorflow_cpu_worker_threads()->workers, line_num,
          line_num, sub_split_func);
  }