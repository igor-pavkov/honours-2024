TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  OneHotContext op_context{context, node};

  if (IsDynamicTensor(op_context.output)) {
    ResizeOutputTensor(context, op_context);
  }

  switch (op_context.output->type) {
    case kTfLiteFloat32:
      OneHotCompute<float>(op_context);
      break;
    case kTfLiteInt32:
      OneHotCompute<int>(op_context);
      break;
    case kTfLiteInt64:
      OneHotCompute<int64_t>(op_context);
      break;
    case kTfLiteInt8:
      OneHotCompute<int8_t>(op_context);
      break;
    case kTfLiteUInt8:
      OneHotCompute<uint8_t>(op_context);
      break;
    case kTfLiteBool:
      OneHotCompute<bool>(op_context);
      break;
    default:
      return kTfLiteError;
  }

  return kTfLiteOk;
}