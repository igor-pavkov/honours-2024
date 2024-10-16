  void Compute(OpKernelContext* context) override {
    const Tensor& input_sizes = context->input(0);
    const Tensor& filter = context->input(1);
    const Tensor& out_backprop = context->input(2);

    TensorShape input_shape;
    OP_REQUIRES_OK(context,
                   Conv2DBackpropComputeInputShape(input_sizes, filter.shape(),
                                                   out_backprop.shape(),
                                                   data_format_, &input_shape));

    Tensor* in_backprop = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(0, input_shape, &in_backprop));

    // If there is nothing to compute, return.
    if (input_shape.num_elements() == 0) {
      return;
    }

    // For now we take the stride from the second and third dimensions only (we
    // do not support striding on the batch or depth dimension).
    const int stride_rows = GetTensorDim(strides_, data_format_, 'H');
    const int stride_cols = GetTensorDim(strides_, data_format_, 'W');
    const int dilation_rows = GetTensorDim(dilations_, data_format_, 'H');
    const int dilation_cols = GetTensorDim(dilations_, data_format_, 'W');

    VLOG(2) << "Conv2DBackpropInput:"
            << " input: " << input_shape.DebugString()
            << " filter:" << filter.shape().DebugString()
            << " out_backprop: " << out_backprop.shape().DebugString()
            << " strides: [" << stride_rows << ", " << stride_cols << "]"
            << " dilations: [" << dilation_rows << ", " << dilation_cols << "]";

    LaunchConv2DBackpropInputOp<Device, T> launch;
    launch(context, use_cudnn_, cudnn_use_autotune_, out_backprop, filter,
           dilation_rows, dilation_cols, stride_rows, stride_cols, padding_,
           explicit_paddings_, in_backprop, data_format_);
  }