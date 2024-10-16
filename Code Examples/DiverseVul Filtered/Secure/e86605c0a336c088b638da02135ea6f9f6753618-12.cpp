  Status DoCompute(OpKernelContext* ctx, const Tensor& i, const Tensor& v,
                   Tensor* y) override {
    const auto& d = ctx->eigen_device<Device>();
    return ::tensorflow::functor::DoInplace(d, op, i, v, y);
  }