  void Compute(OpKernelContext *ctx) override {
    const Tensor *indices_t, *values_t, *shape_t, *reduction_axes_t;
    OP_REQUIRES_OK(ctx, ctx->input("input_indices", &indices_t));
    OP_REQUIRES_OK(ctx, ctx->input("input_values", &values_t));
    OP_REQUIRES_OK(ctx, ctx->input("input_shape", &shape_t));
    OP_REQUIRES_OK(ctx, ctx->input("reduction_axes", &reduction_axes_t));

    OP_REQUIRES_OK(ctx, ValidateInputs(shape_t, reduction_axes_t));

    SparseTensor sp;
    OP_REQUIRES_OK(ctx, SparseTensor::Create(tensor::DeepCopy(*indices_t),
                                         tensor::DeepCopy(*values_t),
                    TensorShape(shape_t->vec<int64>()), &sp));
    ReduceDetails reduction = SparseTensorReduceHelper(
        sp, reduction_axes_t->flat<int32>(), keep_dims_);

    sp.Reorder<T>(reduction.reorder_dims);
    // Count nnzs in the output SparseTensor.
    int64_t nnz = 0;
    auto iter = sp.group(reduction.group_by_dims);
    for (auto it = iter.begin(); it != iter.end(); ++it) {
      nnz++;
    }

    Tensor *out_indices_t;
    OP_REQUIRES_OK(ctx,
                   ctx->allocate_output(
                       0, TensorShape({nnz, reduction.reduced_shape.dims()}),
                       &out_indices_t));
    typename TTypes<int64>::Matrix out_indices_mat =
        out_indices_t->matrix<int64>();
    // For keep_dims. We don't explicitly set dim fields for reduced dims below.
    out_indices_mat.setZero();

    Tensor *out_values_t;
    OP_REQUIRES_OK(ctx,
                   ctx->allocate_output(1, TensorShape({nnz}), &out_values_t));
    auto out_flat = out_values_t->flat<T>();

    Tensor tmp_reduced_val;
    OP_REQUIRES_OK(ctx, ctx->allocate_temp(DataTypeToEnum<T>::value,
                                           TensorShape({}), &tmp_reduced_val));
    auto reduced_val = tmp_reduced_val.scalar<T>();
    int64_t i = 0;
    for (const auto &g : sp.group(reduction.group_by_dims)) {
      Op::template Run<T>(ctx, reduced_val, g.template values<T>());
      std::vector<int64> group = g.group();
      for (int64_t j = 0; j < group.size(); j++) {
        if (keep_dims_) {
          out_indices_mat(i, reduction.group_by_dims[j]) = group[j];
        } else {
          out_indices_mat(i, j) = group[j];
        }
      }
      out_flat(i) = reduced_val();
      i++;
      VLOG(2) << "coords: " << absl::StrJoin(g.group(), ",")
              << "; group " << Op::Name() << ": "
              << reduced_val();
    }

    Tensor *out_shape_t;
    OP_REQUIRES_OK(ctx, ctx->allocate_output(
                            2, TensorShape({reduction.reduced_shape.dims()}),
                            &out_shape_t));
    auto out_shape_flat = out_shape_t->flat<int64>();
    auto out_dim_sizes = reduction.reduced_shape.dim_sizes();
    if (!out_dim_sizes.empty()) {
      std::copy(out_dim_sizes.begin(), out_dim_sizes.end(), &out_shape_flat(0));
    }
  }