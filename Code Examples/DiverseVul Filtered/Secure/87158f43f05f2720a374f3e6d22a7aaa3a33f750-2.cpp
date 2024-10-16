Status ValidateInputs(const Tensor *shape_t, const Tensor *reduction_axes_t) {
  // indices and values are validated in SparseTensor ctor.
  if (!TensorShapeUtils::IsVector(shape_t->shape())) {
    return errors::InvalidArgument(
        "Expected input_shape to be a vector; got shape: ",
        shape_t->shape().DebugString());
  }
  if (!TensorShapeUtils::IsScalar(reduction_axes_t->shape()) &&
      !TensorShapeUtils::IsVector(reduction_axes_t->shape())) {
    return errors::InvalidArgument(
        "Expected reduction_axes to be a scalar or a vector; got shape: ",
        reduction_axes_t->shape().DebugString());
  }

  const auto reduction_axes_flat = reduction_axes_t->flat<int32>();
  for (int64_t i = 0; i < reduction_axes_flat.size(); i++) {
    int32_t axis = reduction_axes_flat(i);
    if (axis < -shape_t->NumElements() || axis >= shape_t->NumElements()) {
      return errors::InvalidArgument("Invalid reduction dimension ", axis,
                                     ", for input with ",
                                     shape_t->NumElements(), " dimensions.");
    }
  }

  return Status::OK();
}