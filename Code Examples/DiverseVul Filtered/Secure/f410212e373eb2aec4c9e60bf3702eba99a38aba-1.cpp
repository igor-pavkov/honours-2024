  void Compute(OpKernelContext* ctx) override {
    const Tensor& arr_t = ctx->input(0);
    const Tensor& size_tensor = ctx->input(1);
    OP_REQUIRES(ctx, size_tensor.dims() == 0,
                errors::InvalidArgument("Shape must be rank 0 but is rank ",
                                        size_tensor.dims()));
    int32_t size = size_tensor.scalar<int32_t>()();
    OP_REQUIRES(
        ctx, size >= 0,
        errors::InvalidArgument("size (", size, ") must be non-negative"));

    const Tensor& weights_t = ctx->input(2);
    const auto arr = arr_t.flat<int32_t>();
    const auto weights = weights_t.flat<T>();
    Tensor* output_t;
    OP_REQUIRES_OK(ctx,
                   ctx->allocate_output(0, TensorShape({size}), &output_t));
    auto output = output_t->flat<T>();
    OP_REQUIRES_OK(ctx,
                   functor::BincountFunctor<Device, int32_t, T, false>::Compute(
                       ctx, arr, weights, output, size));
  }