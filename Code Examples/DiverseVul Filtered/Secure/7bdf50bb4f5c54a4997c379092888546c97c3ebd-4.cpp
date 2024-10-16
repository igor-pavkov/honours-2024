void CompressElementOp::Compute(OpKernelContext* ctx) {
  std::vector<Tensor> components;
  for (size_t i = 0; i < ctx->num_inputs(); ++i) {
    components.push_back(ctx->input(i));
  }
  CompressedElement compressed;
  OP_REQUIRES_OK(ctx, CompressElement(components, &compressed));

  Tensor* output;
  OP_REQUIRES_OK(ctx, ctx->allocate_output(0, TensorShape({}), &output));
  output->scalar<Variant>()() = std::move(compressed);
}