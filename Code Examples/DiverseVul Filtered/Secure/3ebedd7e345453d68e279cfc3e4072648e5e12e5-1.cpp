void OneHotCompute(const OneHotContext& op_context) {
  if (op_context.indices->type == kTfLiteInt64) {
    OneHotComputeImpl<T, int64_t>(op_context);
  } else {
    OneHotComputeImpl<T, int>(op_context);
  }
}