  Status operator()(OpKernelContext *context, const TensorShape &input_shape,
                    const TensorShape &output_shape,
                    typename TTypes<int64>::ConstMatrix input_indices,
                    typename TTypes<int64>::Matrix output_indices) const {
    (void)context;  // Unused (only used in GPU implementation)
    const int64_t input_rank = input_shape.dims();
    const int64_t output_rank = output_shape.dims();
    const int64_t nnz = input_indices.dimension(0);
    gtl::InlinedVector<int64, 8> input_strides(input_rank);
    if (input_rank > 0) {
      input_strides[input_rank - 1] = 1;
      for (int d = input_rank - 2; d >= 0; --d) {
        input_strides[d] = input_strides[d + 1] * input_shape.dim_size(d + 1);
      }
    }

    gtl::InlinedVector<int64, 8> output_strides(output_rank);
    if (output_rank > 0) {
      output_strides[output_rank - 1] = 1;
      for (int d = output_rank - 2; d >= 0; --d) {
        output_strides[d] =
            output_strides[d + 1] * output_shape.dim_size(d + 1);
      }
    }

    for (int i = 0; i < nnz; ++i) {
      int64_t id = 0;
      for (int j = 0; j < input_rank; ++j) {
        id += input_indices(i, j) * input_strides[j];
      }
      for (int j = 0; j < output_rank; ++j) {
        output_indices(i, j) = id / output_strides[j];
        id %= output_strides[j];
      }
    }
    return Status::OK();
  }