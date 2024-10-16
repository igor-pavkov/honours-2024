  bool operator()(OpKernelContext* context, const CPUDevice& d,
                  typename TTypes<float, 4>::Tensor input_backward,
                  typename TTypes<float, 4>::ConstTensor kernel,
                  typename TTypes<float, 4>::ConstTensor output_backward,
                  int input_rows, int input_cols, int row_stride,
                  int col_stride, int pad_h, int pad_w,
                  TensorFormat data_format) const {
    auto batch = input_backward.dimension(0);
    auto in_depth = input_backward.dimension(3);
    auto out_depth = output_backward.dimension(3);
    auto filter_rows = kernel.dimension(0);
    auto filter_cols = kernel.dimension(1);
    auto num_threads =
        context->device()->tensorflow_cpu_worker_threads()->num_threads;
    // See libxsmm_dnn.h for this struct definition.
    libxsmm_dnn_conv_desc desc;
    desc.N = batch;
    desc.C = in_depth;
    desc.H = input_rows;
    desc.W = input_cols;
    desc.K = out_depth;
    desc.R = filter_rows;
    desc.S = filter_cols;
    desc.u = row_stride;
    desc.v = col_stride;
    desc.pad_h = pad_h;
    desc.pad_w = pad_w;
    desc.pad_h_in = 0;
    desc.pad_w_in = 0;
    desc.pad_h_out = 0;
    desc.pad_w_out = 0;
    desc.threads = num_threads;
    desc.algo = LIBXSMM_DNN_CONV_ALGO_DIRECT;
    desc.buffer_format = LIBXSMM_DNN_TENSOR_FORMAT_NHWC;
    desc.filter_format =
        LIBXSMM_DNN_TENSOR_FORMAT_LIBXSMM;  // LIBXSMM_DNN_TENSOR_FORMAT_RSCK;
    desc.fuse_ops = LIBXSMM_DNN_CONV_FUSE_NONE;
    desc.options = LIBXSMM_DNN_CONV_OPTION_OVERWRITE;
    desc.datatype_out = LIBXSMM_DNN_DATATYPE_F32;
    desc.datatype_in = LIBXSMM_DNN_DATATYPE_F32;
    auto input_ptr = input_backward.data();
    auto filter_ptr = kernel.data();
    auto output_ptr = output_backward.data();

    bool success = functor::XsmmBkwInputConv2D<CPUDevice, float>()(
        context, desc, input_ptr, filter_ptr, output_ptr);
    return success;
  }