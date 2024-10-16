  void Compute(OpKernelContext* context) override {
    const Tensor& input = context->input(0);
    const Tensor& sparse_dims = context->input(1);

    if (TensorShapeUtils::IsScalar(input.shape()) || input.NumElements() == 0) {
      context->set_output(0, input);
    } else {
      const int input_dims = input.dims();
      const TensorShape& sparse_dims_shape = sparse_dims.shape();
      const auto& axes_sparse_flat = sparse_dims.flat<Tidx>();

      OP_REQUIRES(context, TensorShapeUtils::IsVector(sparse_dims_shape),
                  errors::InvalidArgument("'dims' must be 1-dimension, not ",
                                          sparse_dims.dims()));
      gtl::InlinedVector<bool, 8> axes_dense(input_dims, false);
      for (int dummy = 0; dummy < axes_sparse_flat.size(); dummy++) {
        Tidx axis = internal::SubtleMustCopy<Tidx>(axes_sparse_flat(dummy));
        Tidx canonical_axis = axis < 0 ? input_dims + axis : axis;
        OP_REQUIRES(context, canonical_axis >= 0 && canonical_axis < input_dims,
                    errors::InvalidArgument("'axis'[", dummy, "] = ", axis,
                                            " is out of valid range [", 0, ", ",
                                            input_dims - 1));
        OP_REQUIRES(context, !axes_dense[canonical_axis],
                    errors::InvalidArgument("axis ", canonical_axis,
                                            " specified more than once."));
        axes_dense[canonical_axis] = true;
      }

      OP_REQUIRES(context, input_dims <= 8,
                  errors::Unimplemented(
                      "reverse is not implemented for tensors of rank > 8."));

      Tensor* output = nullptr;
      OP_REQUIRES_OK(context,
                     context->allocate_output(0, input.shape(), &output));

      // TODO(cwhipkey): we can do dimension folding to reduce, e.g., a reverse
      // of a single dimension to the dims=3 or dims=2 case, regardless of the
      // number of dimensions in the tensor. This would let some ops use faster
      // lower-dimension code (and use optimized versions).

#define HANDLE_REVERSE(NDIMS)                                           \
  case NDIMS:                                                           \
    HandleReverseV2Case<Device, T, NDIMS>(context, axes_dense, output); \
    return;

      switch (input_dims) {
        HANDLE_REVERSE(0);
        HANDLE_REVERSE(1);
        HANDLE_REVERSE(2);
        HANDLE_REVERSE(3);
        HANDLE_REVERSE(4);
        HANDLE_REVERSE(5);
        HANDLE_REVERSE(6);
        HANDLE_REVERSE(7);
        HANDLE_REVERSE(8);
      }
#undef HANDLE_REVERSE
    }
  }