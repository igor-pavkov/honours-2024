Status ConvBackpropComputeDimensions(StringPiece label, int num_spatial_dims,
                                     const TensorShape& input_shape,
                                     const TensorShape& filter_shape,
                                     const TensorShape& out_backprop_shape,
                                     const std::vector<int32>& strides,
                                     Padding padding, TensorFormat data_format,
                                     ConvBackpropDimensions* dims) {
  static constexpr std::array<int32, 5> one_dilations = {{1, 1, 1, 1, 1}};
  return ConvBackpropComputeDimensionsV2(
      label, num_spatial_dims, input_shape, filter_shape, out_backprop_shape,
      one_dilations, strides, padding, /*explicit_paddings=*/{}, data_format,
      dims);
}