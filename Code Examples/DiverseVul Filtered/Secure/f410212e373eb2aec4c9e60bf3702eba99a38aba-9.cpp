  void Compute(OpKernelContext* ctx) override {
    const Tensor& data = ctx->input(0);
    OP_REQUIRES(ctx, data.dims() <= 2,
                errors::InvalidArgument(
                    "Shape must be at most rank 2 but is rank ", data.dims()));

    const Tensor& size_t = ctx->input(1);
    const Tensor& weights = ctx->input(2);

    Tidx size = size_t.scalar<Tidx>()();
    OP_REQUIRES(
        ctx, size >= 0,
        errors::InvalidArgument("size (", size, ") must be non-negative"));

    Tensor* out_t;
    functor::SetZeroFunctor<Device, T> fill;
    if (data.dims() == 1) {
      OP_REQUIRES_OK(ctx, ctx->allocate_output(0, TensorShape({size}), &out_t));
      auto out = out_t->flat<T>();
      fill(ctx->eigen_device<Device>(), out);
      if (binary_output_) {
        OP_REQUIRES_OK(
            ctx, functor::BincountFunctor<Device, Tidx, T, true>::Compute(
                     ctx, data.flat<Tidx>(), weights.flat<T>(), out, size));
      } else {
        OP_REQUIRES_OK(
            ctx, functor::BincountFunctor<Device, Tidx, T, false>::Compute(
                     ctx, data.flat<Tidx>(), weights.flat<T>(), out, size));
      }
    } else if (data.dims() == 2) {
      const int64_t num_rows = data.dim_size(0);
      auto weight_matrix =
          (weights.NumElements() == 0)
              ? weights.shaped<T, 2>(gtl::InlinedVector<int64_t, 2>(2, 0))
              : weights.matrix<T>();
      OP_REQUIRES_OK(
          ctx, ctx->allocate_output(0, TensorShape({num_rows, size}), &out_t));
      auto out = out_t->matrix<T>();
      fill(ctx->eigen_device<Device>(), out_t->flat<T>());
      if (binary_output_) {
        OP_REQUIRES_OK(
            ctx, functor::BincountReduceFunctor<Device, Tidx, T, true>::Compute(
                     ctx, data.matrix<Tidx>(), weight_matrix, out, size));
      } else {
        OP_REQUIRES_OK(
            ctx,
            functor::BincountReduceFunctor<Device, Tidx, T, false>::Compute(
                ctx, data.matrix<Tidx>(), weight_matrix, out, size));
      }
    }
  }