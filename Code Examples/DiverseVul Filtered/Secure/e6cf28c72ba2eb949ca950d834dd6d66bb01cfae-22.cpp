  explicit SparseMatMulOp(OpKernelConstruction* ctx) : OpKernel(ctx) {
    OP_REQUIRES_OK(ctx, ctx->GetAttr("transpose_a", &transpose_a_));
    OP_REQUIRES_OK(ctx, ctx->GetAttr("transpose_b", &transpose_b_));
    OP_REQUIRES_OK(ctx, ctx->GetAttr("a_is_sparse", &a_is_sparse_));
    OP_REQUIRES_OK(ctx, ctx->GetAttr("b_is_sparse", &b_is_sparse_));
  }