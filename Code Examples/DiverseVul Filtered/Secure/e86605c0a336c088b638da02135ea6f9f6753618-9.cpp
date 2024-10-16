  explicit FailureKernel(OpKernelConstruction* ctx) : OpKernel(ctx) {
    OP_REQUIRES_OK(ctx,
                   errors::Internal("Found instance of parallel_stack which "
                                    "could not be properly replaced."));
  }