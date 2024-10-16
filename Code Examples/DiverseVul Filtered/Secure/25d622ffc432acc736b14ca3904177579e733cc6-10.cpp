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
    OP_REQUIRES(
        context,
        FastBoundsCheck(input.NumElements(), std::numeric_limits<int32>::max()),
        errors::InvalidArgument("Split on GPU requires input size "
                                "< max int32"));

    int32_t prefix_dim_size;
    int32_t split_dim_size;
    int32_t suffix_dim_size;
    std::tie(prefix_dim_size, split_dim_size, suffix_dim_size) =
        Base::template SetDims<int32>(input_shape, split_dim);

    // use the same approach as concat (see documentation there)
    // reshape to 2D

    if (num_split > 16) {
      GpuDeviceArrayOnHost<T*> ptrs(context, num_split);
      OP_REQUIRES_OK(context, ptrs.Init());

      GpuDeviceArrayOnHost<Tlen> offsets(context, num_split + 1);
      OP_REQUIRES_OK(context, offsets.Init());

      Tlen offset = 0;
      int entry = split_sizes_vec[0];
      bool fixed_size =
          std::all_of(split_sizes_vec.begin(), split_sizes_vec.end(),
                      [&entry](int n) { return n == entry; });

      for (int i = 0; i < num_split; ++i) {
        TensorShape output_shape(input_shape);
        output_shape.set_dim(split_dim, split_sizes_vec[i]);
        Tensor* result = nullptr;
        OP_REQUIRES_OK(context,
                       context->allocate_output(i, output_shape, &result));
        ptrs.Set(i, result->flat<T>().data());
        offsets.Set(i, offset);
        offset += split_sizes_vec[i] * suffix_dim_size;
      }
      offsets.Set(num_split, offset);
      OP_REQUIRES_OK(context, ptrs.Finalize());
      OP_REQUIRES_OK(context, offsets.Finalize());

      if (input.NumElements() > 0) {
        SplitVOpGPULaunch<T, Tlen>().Run(
            context->eigen_device<GPUDevice>(), fixed_size,
            input.flat<T>().data(), prefix_dim_size,
            input.NumElements() / prefix_dim_size, offsets.data(), ptrs.data());
        OP_REQUIRES(
            context, context->op_device_context()->stream()->ok(),
            errors::Internal("Launch of gpu kernel for SplitVOp failed"));
      }
    } else {
      Eigen::DenseIndex prefix_dim_size;
      Eigen::DenseIndex split_dim_size;
      Eigen::DenseIndex suffix_dim_size;

      std::tie(prefix_dim_size, split_dim_size, suffix_dim_size) =
          Base::template SetDims<Eigen::DenseIndex>(input_shape, split_dim);
      auto input_reshaped = input.shaped<T, 2>(
          {prefix_dim_size, split_dim_size * suffix_dim_size});

      Eigen::DSizes<Eigen::DenseIndex, 2> indices{0, 0};

      for (int i = 0; i < num_split; ++i) {
        TensorShape output_shape(input_shape);
        output_shape.set_dim(split_dim, split_sizes_vec[i]);
        Tensor* result = nullptr;
        OP_REQUIRES_OK(context,
                       context->allocate_output(i, output_shape, &result));

        Eigen::DSizes<Eigen::DenseIndex, 2> sizes{
            prefix_dim_size, split_sizes_vec[i] * suffix_dim_size};

        if (sizes.TotalSize() > 0) {
          auto result_shaped = result->shaped<T, 2>(
              {prefix_dim_size, split_sizes_vec[i] * suffix_dim_size});

          functor::SplitCustom<GPUDevice, T>()(
              context->eigen_device<GPUDevice>(), result_shaped, input_reshaped,
              indices, sizes);
        }
        indices[1] += split_sizes_vec[i] * suffix_dim_size;
      }
    }
  }