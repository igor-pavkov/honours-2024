void HandleReverseCase(OpKernelContext* context,
                       typename TTypes<bool, 1>::ConstTensor dims,
                       Tensor* result) {
  const Tensor& input = context->input(0);

  // Use optimized reverse if possible.
  if (NDIMS == 3 && std::is_same<Device, CPUDevice>::value &&
      data_type_can_memcpy<T>::value && (!dims(0) && dims(1) && !dims(2))) {
    if (input.dim_size(2) == 3) {
      DoHandleReverseCase<T, 3>(context, input, result);
    } else {
      DoHandleReverseCase<T, -1>(context, input, result);
    }
    return;
  }
  typename Eigen::array<bool, NDIMS> axes_di;
  for (int i = 0; i < NDIMS; i++) {
    axes_di[i] = dims(i);
  }
  functor::Reverse<Device, T, NDIMS>()(context->eigen_device<Device>(),
                                       input.tensor<T, NDIMS>(), axes_di,
                                       result->tensor<T, NDIMS>());
}