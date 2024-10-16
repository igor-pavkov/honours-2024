TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  SpaceToBatchNDContext op_context(context, node);

  // Resize the output tensor if the output tensor is dynamic.
  if (IsDynamicTensor(op_context.output)) {
    TF_LITE_ENSURE_OK(context, ResizeOutputTensor(context, &op_context));
  }

#define TF_LITE_SPACE_TO_BATCH_ND(type, scalar, pad_value)             \
  tflite::SpaceToBatchParams op_params;                                \
  op_params.output_offset = pad_value;                                 \
  type::SpaceToBatchND(op_params, GetTensorShape(op_context.input),    \
                       GetTensorData<scalar>(op_context.input),        \
                       GetTensorShape(op_context.block_shape),         \
                       GetTensorData<int32_t>(op_context.block_shape), \
                       GetTensorShape(op_context.paddings),            \
                       GetTensorData<int32_t>(op_context.paddings),    \
                       GetTensorShape(op_context.output),              \
                       GetTensorData<scalar>(op_context.output))
  switch (op_context.input->type) {  // Already know in/out types are same.
    case kTfLiteFloat32:
      if (kernel_type == kReference) {
        TF_LITE_SPACE_TO_BATCH_ND(reference_ops, float, 0);
      } else {
        TF_LITE_SPACE_TO_BATCH_ND(optimized_ops, float, 0);
      }
      break;
    case kTfLiteUInt8:
      if (kernel_type == kReference) {
        TF_LITE_SPACE_TO_BATCH_ND(reference_ops, uint8_t,
                                  op_context.output->params.zero_point);
      } else {
        TF_LITE_SPACE_TO_BATCH_ND(optimized_ops, uint8_t,
                                  op_context.output->params.zero_point);
      }
      break;
    case kTfLiteInt8:
      if (kernel_type == kReference) {
        TF_LITE_SPACE_TO_BATCH_ND(reference_ops, int8_t,
                                  op_context.output->params.zero_point);
      } else {
        TF_LITE_SPACE_TO_BATCH_ND(optimized_ops, int8_t,
                                  op_context.output->params.zero_point);
      }
      break;
    case kTfLiteInt32:
      if (kernel_type == kReference) {
        TF_LITE_SPACE_TO_BATCH_ND(reference_ops, int32_t, 0);
      } else {
        TF_LITE_SPACE_TO_BATCH_ND(optimized_ops, int32_t, 0);
      }
      break;
    case kTfLiteInt64:
      if (kernel_type == kReference) {
        TF_LITE_SPACE_TO_BATCH_ND(reference_ops, int64_t, 0);
      } else {
        TF_LITE_SPACE_TO_BATCH_ND(optimized_ops, int64_t, 0);
      }
      break;
    default:
      context->ReportError(
          context, "Type %d is currently not supported by SpaceToBatch.",
          op_context.input->type);
      return kTfLiteError;
  }
#undef TF_LITE_SPACE_TO_BATCH_ND
  return kTfLiteOk;
}