  void operator()(OpKernelContext* ctx, bool use_cudnn, bool cudnn_use_autotune,
                  const Tensor& out_backprop, const Tensor& filter,
                  int row_dilation, int col_dilation, int row_stride,
                  int col_stride, const Padding& padding,
                  const std::vector<int64>& explicit_paddings,
                  Tensor* in_backprop, TensorFormat data_format) {
    std::vector<int32> strides(4, 1);
    std::vector<int32> dilations(4, 1);

    auto input_h = GetTensorDimIndex(data_format, 'H');
    auto input_w = GetTensorDimIndex(data_format, 'W');
    strides[input_h] = row_stride;
    strides[input_w] = col_stride;
    dilations[input_h] = row_dilation;
    dilations[input_w] = col_dilation;

    const TensorShape& input_shape = in_backprop->shape();
    const TensorShape& filter_shape = filter.shape();

    ConvBackpropDimensions dims;
    OP_REQUIRES_OK(
        ctx, ConvBackpropComputeDimensionsV2(
                 "Conv2DBackpropInput", /*num_spatial_dims=*/2, input_shape,
                 filter_shape, out_backprop.shape(), dilations, strides,
                 padding, explicit_paddings, data_format, &dims));

    int64 padding_top = -1, padding_bottom = -1;
    int64 padding_left = -1, padding_right = -1;
    if (padding == EXPLICIT) {
      GetExplicitPaddingForDim(explicit_paddings, data_format, 'H',
                               &padding_top, &padding_bottom);
      GetExplicitPaddingForDim(explicit_paddings, data_format, 'W',
                               &padding_left, &padding_right);
    }

    int64 expected_out_rows, expected_out_cols;
    // The function is guaranteed to succeed because we checked the output and
    // padding was valid earlier.
    TF_CHECK_OK(GetWindowedOutputSizeVerboseV2(
        dims.spatial_dims[0].input_size, dims.spatial_dims[0].filter_size,
        row_dilation, row_stride, padding, &expected_out_rows, &padding_top,
        &padding_bottom));
    DCHECK_EQ(dims.spatial_dims[0].output_size, expected_out_rows);

    TF_CHECK_OK(GetWindowedOutputSizeVerboseV2(
        dims.spatial_dims[1].input_size, dims.spatial_dims[1].filter_size,
        col_dilation, col_stride, padding, &expected_out_cols, &padding_left,
        &padding_right));
    DCHECK_EQ(dims.spatial_dims[1].output_size, expected_out_cols);

    if (std::is_same<Device, GPUDevice>::value) {
      int64 size = 1;
#define REQUIRES_32BIT(x)                                                   \
  size *= x;                                                                \
  OP_REQUIRES(ctx,                                                          \
              FastBoundsCheck(x, std::numeric_limits<int32>::max()) &&      \
                  FastBoundsCheck(size, std::numeric_limits<int32>::max()), \
              errors::InvalidArgument("Tensor too large"))

      REQUIRES_32BIT(in_backprop->dim_size(0));
      REQUIRES_32BIT(in_backprop->dim_size(1) + padding_top + padding_bottom);
      REQUIRES_32BIT(in_backprop->dim_size(2) + padding_left + padding_right);
      REQUIRES_32BIT(in_backprop->dim_size(3));
#undef REQUIRES_32BIT
    }

    auto in_backprop_t = in_backprop->tensor<T, 4>();
    auto out_backprop_t = out_backprop.tensor<T, 4>();
    auto filter_t = filter.tensor<T, 4>();

    // WARNING: Need to swap row/col, padding_top/padding_left, and
    // padding_bottom/padding_right when calling Eigen. Eigen expects tensors
    // in NWHC format, but Tensorflow uses NHWC.

    if (padding != EXPLICIT) {
      // If padding was not explicitly defined, Eigen spatial convolution
      // backward input will infer correct forward paddings from input tensors.
      functor::SpatialConvolutionBackwardInputFunc<Device, T>()(
          ctx->eigen_device<Device>(), in_backprop_t, filter_t, out_backprop_t,
          col_stride, row_stride, col_dilation, row_dilation);
    } else {
      functor::SpatialConvolutionBackwardInputWithExplicitPaddingFunc<Device,
                                                                      T>()(
          ctx->eigen_device<Device>(), in_backprop_t, filter_t, out_backprop_t,
          in_backprop_t.dimension(2) + (padding_left + padding_right),
          in_backprop_t.dimension(1) + (padding_top + padding_bottom),
          col_stride, row_stride, col_dilation, row_dilation, padding_top,
          padding_left);
    }
  }