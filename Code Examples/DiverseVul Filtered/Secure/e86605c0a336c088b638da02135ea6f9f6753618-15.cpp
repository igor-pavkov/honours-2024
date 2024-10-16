  void Compute(OpKernelContext* ctx) override {
    auto x = ctx->input(0);
    Tensor* y;
    OP_REQUIRES_OK(ctx, ctx->allocate_output(0, x.shape(), &y));
    OP_REQUIRES_OK(ctx, DoCompute(ctx, x, y));
  }