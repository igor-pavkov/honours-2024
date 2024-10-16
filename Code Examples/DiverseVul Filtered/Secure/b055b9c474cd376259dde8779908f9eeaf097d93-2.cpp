  explicit RaggedTensorToVariantOp(OpKernelConstruction* context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("batched_input", &batched_input_));
  }