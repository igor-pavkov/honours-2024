  explicit Dataset(OpKernelContext* ctx,
                   const sparse::SparseTensor& sparse_tensor)
      : DatasetBase(DatasetContext(ctx)),
        sparse_tensor_(sparse_tensor),
        dtypes_({DT_INT64, sparse_tensor.dtype(), DT_INT64}),
        shapes_({{-1, sparse_tensor.dims() - 1},
                 {-1},
                 {sparse_tensor.dims() - 1}}) {}