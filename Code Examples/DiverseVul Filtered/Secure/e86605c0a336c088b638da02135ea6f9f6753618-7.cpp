  Status DoCompute(OpKernelContext* ctx, const Tensor& x, Tensor* y) override {
    const auto& d = ctx->eigen_device<Device>();
    return ::tensorflow::functor::DoCopy(d, x, y);
  }