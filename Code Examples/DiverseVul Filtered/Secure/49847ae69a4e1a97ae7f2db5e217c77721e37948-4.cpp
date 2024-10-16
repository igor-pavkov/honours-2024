inline Padding3DValues ComputePadding3DValues(
    int stride_height, int stride_width, int stride_depth,
    int dilation_rate_height, int dilation_rate_width, int dilation_rate_depth,
    int in_height, int in_width, int in_depth, int filter_height,
    int filter_width, int filter_depth, TfLitePadding padding, int* out_height,
    int* out_width, int* out_depth) {
  *out_width = ComputeOutSize(padding, in_width, filter_width, stride_width,
                              dilation_rate_width);
  *out_height = ComputeOutSize(padding, in_height, filter_height, stride_height,
                               dilation_rate_height);
  *out_depth = ComputeOutSize(padding, in_depth, filter_depth, stride_depth,
                              dilation_rate_depth);

  Padding3DValues padding_values;
  int offset = 0;
  padding_values.depth =
      ComputePaddingWithOffset(stride_depth, dilation_rate_depth, in_depth,
                               filter_depth, *out_depth, &offset);
  padding_values.depth_offset = offset;
  padding_values.height =
      ComputePaddingWithOffset(stride_height, dilation_rate_height, in_height,
                               filter_height, *out_height, &offset);
  padding_values.height_offset = offset;
  padding_values.width =
      ComputePaddingWithOffset(stride_width, dilation_rate_width, in_width,
                               filter_width, *out_width, &offset);
  padding_values.width_offset = offset;
  return padding_values;
}