  static void Run(OpKernelContext *ctx, typename TTypes<T>::Scalar &s, const typename TTypes<T>::UnalignedVec &v) {
      s.device(ctx->eigen_cpu_device()) = v.sum();
  }