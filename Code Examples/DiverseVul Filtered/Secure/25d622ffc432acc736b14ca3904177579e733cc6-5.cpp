  void Compute(OpKernelContext* context) override {
    bool done = false;
    std::vector<Tlen> split_sizes_vec;
    Base::ComputeEasyCases(context, &done, &split_sizes_vec);
    if (!context->status().ok() || done) {
      return;
    }
    const int32_t num_split = Base::num_outputs();
    const Tensor& input = context->input(0);
    const TensorShape& input_shape = input.shape();
    const int32_t split_dim_orig = context->input(2).flat<int32>()(0);
    const int32_t split_dim =
        split_dim_orig < 0 ? split_dim_orig + input.dims() : split_dim_orig;

    // Android also uses int32 indexing, so check here also.
    OP_REQUIRES(
        context,
        FastBoundsCheck(input.NumElements(),
                        std::numeric_limits<Eigen::DenseIndex>::max()),
        errors::InvalidArgument("Split requires input size < ",
                                std::numeric_limits<Eigen::DenseIndex>::max()));

    Eigen::DenseIndex prefix_dim_size;
    Eigen::DenseIndex split_dim_size;
    Eigen::DenseIndex suffix_dim_size;

    std::tie(prefix_dim_size, split_dim_size, suffix_dim_size) =
        Base::template SetDims<Eigen::DenseIndex>(input_shape, split_dim);
    std::vector<int64_t> split_start_points(num_split);
    for (int i = 0; i < num_split; ++i) {
      if (i == 0) {
        split_start_points[i] = 0;
      } else {
        split_start_points[i] =
            split_start_points[i - 1] + split_sizes_vec[i - 1];
      }
    }

    if (prefix_dim_size == 1) {
      auto input_reshaped =
          input.shaped<T, 2>({split_dim_size, suffix_dim_size});
      auto make_sizes = [&](Eigen::DenseIndex split_size) {
        return Eigen::DSizes<Eigen::DenseIndex, 2>{split_size, suffix_dim_size};
      };
      auto reshape_result = [&](Tensor* result, Tlen split_size) {
        return result->shaped<T, 2>({split_size, suffix_dim_size});
      };
      SplitVOpCPUImpl<T, Tlen, decltype(input_reshaped), 2>{}(
          context, input_reshaped, split_start_points, input_shape, split_dim,
          prefix_dim_size, split_dim_size, suffix_dim_size, split_sizes_vec,
          make_sizes, reshape_result);
    } else {
      auto input_reshaped = input.shaped<T, 3>(
          {prefix_dim_size, split_dim_size, suffix_dim_size});
      auto make_sizes = [&](Eigen::DenseIndex split_size) {
        return Eigen::DSizes<Eigen::DenseIndex, 3>{prefix_dim_size, split_size,
                                                   suffix_dim_size};
      };
      auto reshape_result = [&](Tensor* result, Tlen split_size) {
        return result->shaped<T, 3>(
            {prefix_dim_size, split_size, suffix_dim_size});
      };
      SplitVOpCPUImpl<T, Tlen, decltype(input_reshaped), 3>{}(
          context, input_reshaped, split_start_points, input_shape, split_dim,
          prefix_dim_size, split_dim_size, suffix_dim_size, split_sizes_vec,
          make_sizes, reshape_result);
    }
  }