Status Conv2DBackpropComputeInputShape(const Tensor& input_sizes,
                                       const TensorShape& filter_shape,
                                       const TensorShape& out_backprop_shape,
                                       const TensorFormat& data_format,
                                       TensorShape* input_shape) {
  if (!TensorShapeUtils::IsVector(input_sizes.shape())) {
    return errors::InvalidArgument(
        "Conv2DBackpropInput: input_sizes input must be 1-dim, not ",
        input_sizes.dims());
  }

  if (input_sizes.dim_size(0) == 4) {
    return TensorShapeUtils::MakeShape(input_sizes.vec<int32>(), input_shape);
  }

  if (input_sizes.dim_size(0) == 2) {
    const int batch_size = GetTensorDim(out_backprop_shape, data_format, 'N');
    const int output_height = input_sizes.vec<int32>()(0);
    const int output_width = input_sizes.vec<int32>()(1);
    const int output_depth = filter_shape.dim_size(2);
    *input_shape = ShapeFromFormat(data_format, batch_size, output_height,
                                   output_width, output_depth);
    return Status::OK();
  }

  return errors::InvalidArgument(
      "Conv2DBackpropInput requires input_sizes to "
      "contain 4 values or 2 values, but got: ",
      input_sizes.dim_size(0));
}