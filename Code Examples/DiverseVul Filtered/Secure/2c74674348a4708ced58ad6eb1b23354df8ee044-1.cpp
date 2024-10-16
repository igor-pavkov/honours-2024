TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  BatchToSpaceNDContext op_context(context, node);

  // Resize the output tensor if the output tensor is dynamic.
  if (IsDynamicTensor(op_context.output)) {
    TF_LITE_ENSURE_OK(context, ResizeOutputTensor(context, &op_context));
  }

#define TF_LITE_BATCH_TO_SPACE_ND(type, scalar)                        \
  type::BatchToSpaceND(GetTensorShape(op_context.input),               \
                       GetTensorData<scalar>(op_context.input),        \
                       GetTensorShape(op_context.block_shape),         \
                       GetTensorData<int32_t>(op_context.block_shape), \
                       GetTensorShape(op_context.crops),               \
                       GetTensorData<int32_t>(op_context.crops),       \
                       GetTensorShape(op_context.output),              \
                       GetTensorData<scalar>(op_context.output))
  switch (op_context.input->type) {  // Already know in/out types are same.
    case kTfLiteFloat32:
      if (kernel_type == kReference) {
        TF_LITE_BATCH_TO_SPACE_ND(reference_ops, float);
      } else {
        TF_LITE_BATCH_TO_SPACE_ND(optimized_ops, float);
      }
      break;
    case kTfLiteUInt8:
      if (kernel_type == kReference) {
        TF_LITE_BATCH_TO_SPACE_ND(reference_ops, uint8_t);
      } else {
        TF_LITE_BATCH_TO_SPACE_ND(optimized_ops, uint8_t);
      }
      break;
    case kTfLiteInt8:
      if (kernel_type == kReference) {
        TF_LITE_BATCH_TO_SPACE_ND(reference_ops, int8_t);
      } else {
        TF_LITE_BATCH_TO_SPACE_ND(optimized_ops, int8_t);
      }
      break;
    case kTfLiteInt32:
      if (kernel_type == kReference) {
        TF_LITE_BATCH_TO_SPACE_ND(reference_ops, int32_t);
      } else {
        TF_LITE_BATCH_TO_SPACE_ND(optimized_ops, int32_t);
      }
      break;
    case kTfLiteInt64:
      if (kernel_type == kReference) {
        TF_LITE_BATCH_TO_SPACE_ND(reference_ops, int64_t);
      } else {
        TF_LITE_BATCH_TO_SPACE_ND(optimized_ops, int64_t);
      }
      break;
    default:
      context->ReportError(
          context, "Type %d is currently not supported by BatchToSpace.",
          op_context.input->type);
      return kTfLiteError;
  }
#undef TF_LITE_BATCH_TO_SPACE_ND
  return kTfLiteOk;
}