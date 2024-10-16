  bool operator()(OpKernelContext* context, const Device& d,
                  typename TTypes<T, 4>::Tensor input_backward,
                  typename TTypes<T, 4>::ConstTensor kernel,
                  typename TTypes<T, 4>::ConstTensor output_backward,
                  int input_rows, int input_cols, int row_stride,
                  int col_stride, int pad_h, int pad_w,
                  TensorFormat data_format) const {
    return false;
  }