Status ConvBackpropExtractAndVerifyDimension(
    StringPiece label, const TensorShape& input_shape,
    const TensorShape& filter_shape, const TensorShape& output_shape,
    const gtl::ArraySlice<int32> dilations, const std::vector<int32>& strides,
    Padding padding, int64 padding_before, int64 padding_after, int spatial_dim,
    int filter_spatial_dim, ConvBackpropSpatialDimension* dim) {
  dim->input_size = input_shape.dim_size(spatial_dim);
  dim->filter_size = filter_shape.dim_size(filter_spatial_dim);
  dim->output_size = output_shape.dim_size(spatial_dim);
  dim->stride = strides[spatial_dim];
  dim->dilation = dilations[spatial_dim];
  int64 out_size = 0;
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeVerboseV2(
      dim->input_size, dim->filter_size, dim->dilation, dim->stride, padding,
      &out_size, &padding_before, &padding_after));
  if (dim->output_size != out_size) {
    return errors::InvalidArgument(
        label, ": Size of out_backprop doesn't match computed: ", "actual = ",
        dim->output_size, ", computed = ", out_size,
        " spatial_dim: ", spatial_dim, " input: ", dim->input_size,
        " filter: ", dim->filter_size, " output: ", dim->output_size,
        " stride: ", dim->stride, " dilation: ", dim->dilation);
  }

  int64 effective_filter_size = (dim->filter_size - 1) * dim->dilation + 1;
  dim->expanded_output_size = (dim->output_size - 1) * dim->stride + 1;
  const auto padded_out_size = dim->input_size + effective_filter_size - 1;
  dim->pad_before = effective_filter_size - 1 - padding_before;
  dim->pad_after =
      padded_out_size - dim->expanded_output_size - dim->pad_before;
  VLOG(2) << label << ": expanded_out = " << dim->expanded_output_size
          << ", effective_filter_size = " << effective_filter_size
          << ", padded_out = " << padded_out_size
          << ", pad_before = " << dim->pad_before
          << ", pad_after = " << dim->pad_after
          << ", dilation = " << dim->dilation << ", strides = " << dim->stride;
  return Status::OK();
}