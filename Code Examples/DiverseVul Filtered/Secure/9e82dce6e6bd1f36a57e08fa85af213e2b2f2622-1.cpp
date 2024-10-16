  Status run(BundleReader* reader) {
    TensorShape restored_full_shape;
    TF_RETURN_IF_ERROR(
        reader->LookupTensorShape(tensor_name, &restored_full_shape));

    VLOG(1) << "Restoring tensor " << idx << " : " << tensor_name << " : "
            << restored_full_shape.num_elements();
    Tensor* restored_tensor;
    if (shape_and_slice.empty()) {
      // Lookup the full tensor.
      TF_RETURN_IF_ERROR(
          context->allocate_output(idx, restored_full_shape, &restored_tensor));
      TF_RETURN_IF_ERROR(reader->Lookup(tensor_name, restored_tensor));
    } else {
      // Lookup the slice.
      TensorShape parsed_full_shape;
      TensorSlice parsed_slice;
      TensorShape parsed_slice_shape;

      TF_RETURN_IF_ERROR(
          checkpoint::ParseShapeAndSlice(shape_and_slice, &parsed_full_shape,
                                         &parsed_slice, &parsed_slice_shape));

      if (!restored_full_shape.IsSameSize(parsed_full_shape)) {
        return errors::InvalidArgument(
            "tensor_name = ", tensor_name, "; shape in shape_and_slice spec ",
            parsed_full_shape.DebugString(),
            " does not match the shape stored in checkpoint: ",
            restored_full_shape.DebugString());
      }
      TF_RETURN_IF_ERROR(
          context->allocate_output(idx, parsed_slice_shape, &restored_tensor));
      TF_RETURN_IF_ERROR(
          reader->LookupSlice(tensor_name, parsed_slice, restored_tensor));
    }
    if (VLOG_IS_ON(5)) {
      if (restored_tensor->dtype() == DT_FLOAT) {
        const float* t_data = restored_tensor->flat<float>().data();
        float min = std::numeric_limits<float>::infinity();
        float max = -std::numeric_limits<float>::infinity();
        double avg = 0.0;
        for (int i = 0; i < restored_tensor->NumElements(); ++i) {
          if (t_data[i] < min) min = t_data[i];
          if (t_data[i] > max) max = t_data[i];
          avg += t_data[i];
        }
        VLOG(5) << " min " << min << " max " << max << " avg "
                << avg / restored_tensor->NumElements() << " total elts "
                << restored_tensor->NumElements();
      }
    }
    VLOG(1) << "Done restoring tensor " << idx << " : " << tensor_name << " : "
            << restored_full_shape.num_elements();
    return Status::OK();
  }