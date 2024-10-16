  explicit MklRequantizePerChannelOp(OpKernelConstruction* ctx)
      : OpKernel(ctx) {
    OP_REQUIRES_OK(ctx, ctx->GetAttr("out_type", &out_type_));
    OP_REQUIRES(ctx, out_type_ == DT_QINT8 || out_type_ == DT_QUINT8,
                errors::InvalidArgument(
                    "out_type must be qint8 or quint8, but got: ", out_type_));
  }