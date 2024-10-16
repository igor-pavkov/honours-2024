  explicit ParallelConcatUpdate(OpKernelConstruction* ctx) : OpKernel(ctx) {
    OP_REQUIRES_OK(ctx, ctx->GetAttr("loc", &loc_));
  }