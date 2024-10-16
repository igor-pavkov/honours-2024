  void Compute(OpKernelContext* ctx) override {
    Tensor* out = nullptr;
    // We do not know whether the output will be used on GPU. Setting it to be
    // gpu-compatible for now.
    AllocatorAttributes attr;
    attr.set_gpu_compatible(true);
    OP_REQUIRES_OK(ctx, ctx->allocate_output(0, shape_, &out, attr));
  }