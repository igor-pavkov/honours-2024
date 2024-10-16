Status RestoreTensorsV2(OpKernelContext* context, const Tensor& prefix,
                        const Tensor& tensor_names,
                        const Tensor& shape_and_slices,
                        gtl::ArraySlice<DataType> dtypes) {
  const string& prefix_string = prefix.scalar<tstring>()();

  const auto& tensor_names_flat = tensor_names.flat<tstring>();
  const auto& shape_and_slices_flat = shape_and_slices.flat<tstring>();

  // Sort lookup keys to improve locality when reading multiple tensors.
  std::vector<size_t> sorted_name_idx(tensor_names_flat.size());
  std::iota(sorted_name_idx.begin(), sorted_name_idx.end(), 0);
  std::sort(sorted_name_idx.begin(), sorted_name_idx.end(),
            [&tensor_names_flat](size_t a, size_t b) {
              return tensor_names_flat(a) < tensor_names_flat(b);
            });

  std::vector<std::unique_ptr<RestoreOp> > pool_restore_ops;
  std::vector<std::unique_ptr<RestoreOp> > direct_restore_ops;

  BundleReader default_reader(Env::Default(), prefix_string);
  TF_RETURN_IF_ERROR(default_reader.status());

  std::vector<string> mismatched_errors;
  for (const size_t i : sorted_name_idx) {
    TensorShape restored_full_shape;
    DataType original_dtype;
    const string& tensor_name = tensor_names_flat(i);
    TF_RETURN_IF_ERROR(default_reader.LookupDtypeAndShape(
        tensor_name, &original_dtype, &restored_full_shape));
    if (dtypes[i] != original_dtype) {
      string error_msg = strings::StrCat(
          "tensor_name = ", tensor_name, "; expected dtype ",
          DataTypeString(dtypes[i]), " does not equal original dtype ",
          DataTypeString(original_dtype));
      mismatched_errors.emplace_back(error_msg);
    }
  }
  if (!mismatched_errors.empty()) {
    const string error_msg = absl::StrJoin(mismatched_errors, "\n");
    return errors::InvalidArgument(error_msg);
  }

  for (auto i : sorted_name_idx) {
    const string& tensor_name = tensor_names_flat(i);
    const string& shape_and_slice = shape_and_slices_flat(i);
    auto op =
        new RestoreOp{context, i, tensor_name, shape_and_slice, prefix_string};
    if (op->should_run_in_pool(&default_reader)) {
      pool_restore_ops.emplace_back(op);
    } else {
      direct_restore_ops.emplace_back(op);
    }
  }

  {
    // Schedule any threaded operations first, skipping thread pool creation if
    // we don't have any expensive operations.
    std::unique_ptr<thread::ThreadPool> reader_pool;
    if (!pool_restore_ops.empty()) {
      reader_pool.reset(
          new thread::ThreadPool(Env::Default(), "restore_tensors", 8));
      for (auto& op : pool_restore_ops) {
        reader_pool->Schedule([&op]() { op->run_with_new_reader(); });
      }
    }

    // Read small tensors from the op thread
    for (auto& op : direct_restore_ops) {
      TF_RETURN_IF_ERROR(op->run(&default_reader));
    }
  }

  // Check status of pool ops; this must come after the pool shuts down.
  for (auto& op : pool_restore_ops) {
    TF_RETURN_IF_ERROR(op->status);
  }

  for (auto i : sorted_name_idx) {
    const string& tensor_name = tensor_names_flat(i);
    if (dtypes[i] != context->mutable_output(i)->dtype()) {
      return errors::InvalidArgument(
          "tensor_name = ", tensor_name, "; expected dtype ",
          DataTypeString(dtypes[i]), " does not equal restored dtype ",
          DataTypeString(context->mutable_output(i)->dtype()));
    }
  }

  return Status::OK();
}