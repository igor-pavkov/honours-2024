TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  TF_LITE_ENSURE_EQ(context, NumInputs(node), 4);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  OneHotContext op_context{context, node};
  switch (op_context.dtype) {
    // TODO(b/111744875): Support uint8 and quantization.
    case kTfLiteFloat32:
    case kTfLiteInt16:
    case kTfLiteInt32:
    case kTfLiteInt64:
    case kTfLiteInt8:
    case kTfLiteUInt8:
    case kTfLiteBool:
      op_context.output->type = op_context.dtype;
      break;
    default:
      TF_LITE_KERNEL_LOG(context, "Unknown output data type: %s",
                         TfLiteTypeGetName(op_context.dtype));
      return kTfLiteError;
  }

  TF_LITE_ENSURE(context, op_context.indices->type == kTfLiteInt32 ||
                              op_context.indices->type == kTfLiteInt64);
  TF_LITE_ENSURE(context, op_context.axis >= 0 &&
                              op_context.axis < op_context.output_dims);
  TF_LITE_ENSURE_EQ(context, NumElements(op_context.depth), 1);
  TF_LITE_ENSURE_EQ(context, NumElements(op_context.on_value), 1);
  TF_LITE_ENSURE_EQ(context, NumElements(op_context.off_value), 1);
  TF_LITE_ENSURE_TYPES_EQ(context, op_context.on_value->type, op_context.dtype);
  TF_LITE_ENSURE_TYPES_EQ(context, op_context.off_value->type,
                          op_context.dtype);

  if (!IsConstantTensor(op_context.depth)) {
    SetTensorToDynamic(op_context.output);
    return kTfLiteOk;
  }

  return ResizeOutputTensor(context, op_context);
}