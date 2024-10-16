  explicit QuantizedConv2DOp(OpKernelConstruction* context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("strides", &strides_));
    OP_REQUIRES(context, strides_.size() == 4,
                errors::InvalidArgument("Sliding window strides field must "
                                        "specify 4 dimensions"));
    OP_REQUIRES(context, strides_[1] == strides_[2],
                errors::InvalidArgument(
                    "Current implementation only supports equal length "
                    "strides in the row and column dimensions."));
    OP_REQUIRES(
        context, (strides_[0] == 1 && strides_[3] == 1),
        errors::InvalidArgument("Current implementation does not yet support "
                                "strides in the batch and depth dimensions."));
    std::vector<int32> dilations;
    OP_REQUIRES_OK(context, context->GetAttr("dilations", &dilations));
    OP_REQUIRES(context, dilations.size() == 4,
                errors::InvalidArgument("Dilations field must "
                                        "specify 4 dimensions"));
    OP_REQUIRES(context, dilations[1] == 1 && dilations[2] == 1,
                errors::InvalidArgument(
                    "Current implementation only supports dilated rate as 1 "
                    "in the row and column dimensions."));
    OP_REQUIRES(context, (dilations[0] == 1 && dilations[3] == 1),
                errors::InvalidArgument(
                    "Current implementation does not yet support "
                    "dilations in the batch and depth dimensions."));
    OP_REQUIRES_OK(context, context->GetAttr("padding", &padding_));
  }