  explicit DenseBincountOp(OpKernelConstruction* ctx) : OpKernel(ctx) {
    OP_REQUIRES_OK(ctx, ctx->GetAttr("binary_output", &binary_output_));
    if (std::is_same<Device, GPUDevice>::value) {
      OP_REQUIRES(
          ctx, !OpDeterminismRequired(),
          errors::Unimplemented(
              "Determinism is not yet supported in GPU implementation of "
              "DenseBincount."));
    }
  }