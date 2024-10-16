  explicit QuantizedBatchNormOp(OpKernelConstruction* context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context,
                   context->GetAttr("variance_epsilon", &variance_epsilon_));
    OP_REQUIRES_OK(context, context->GetAttr("scale_after_normalization",
                                             &scale_after_normalization_));
  }