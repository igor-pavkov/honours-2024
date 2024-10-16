  void operator()(OpKernelContext* ctx, const T* out_data, const T* filter_data,
                  const int filter_total_size, const int output_image_size,
                  const int dims_out_depth, T* im2col_buf) {
    // Compute gradient into 'im2col_buf'.
    MatrixMap C(im2col_buf, output_image_size, filter_total_size);

    ConstMatrixMap A(out_data, output_image_size, dims_out_depth);
    ConstMatrixMap B(filter_data, filter_total_size, dims_out_depth);

    C.noalias() = A * B.transpose();
  }