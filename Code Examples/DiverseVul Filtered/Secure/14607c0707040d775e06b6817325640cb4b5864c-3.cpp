  explicit CTCLossOpGPU(OpKernelConstruction* ctx) : OpKernel(ctx) {
    bool preprocess_collapse_repeated;
    bool ctc_merge_repeated;
    bool ignore_longer_outputs_than_inputs;
    OP_REQUIRES_OK(ctx, ctx->GetAttr("preprocess_collapse_repeated",
                                     &preprocess_collapse_repeated));
    OP_REQUIRES_OK(ctx,
                   ctx->GetAttr("ctc_merge_repeated", &ctc_merge_repeated));
    OP_REQUIRES_OK(ctx, ctx->GetAttr("ignore_longer_outputs_than_inputs",
                                     &ignore_longer_outputs_than_inputs));

    OP_REQUIRES(ctx, !preprocess_collapse_repeated,
                errors::InvalidArgument("GPU CTCLossOp requires "
                                        "preprocess_collapse_repeated to be "
                                        "false"));
    OP_REQUIRES(ctx, ctc_merge_repeated,
                errors::InvalidArgument("GPU CTCLossOp requires "
                                        "ctc_merge_repeated to be "
                                        "true"));
    OP_REQUIRES(ctx, !ignore_longer_outputs_than_inputs,
                errors::InvalidArgument("GPU CTCLossOp requires "
                                        "ignore_longer_outputs_than_inputs to"
                                        "be false"));
  }