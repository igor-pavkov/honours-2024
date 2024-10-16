  explicit CTCLossOp(OpKernelConstruction* ctx) : OpKernel(ctx) {
    OP_REQUIRES_OK(ctx, ctx->GetAttr("preprocess_collapse_repeated",
                                     &preprocess_collapse_repeated_));
    OP_REQUIRES_OK(ctx,
                   ctx->GetAttr("ctc_merge_repeated", &ctc_merge_repeated_));
    OP_REQUIRES_OK(ctx, ctx->GetAttr("ignore_longer_outputs_than_inputs",
                                     &ignore_longer_outputs_than_inputs_));
  }