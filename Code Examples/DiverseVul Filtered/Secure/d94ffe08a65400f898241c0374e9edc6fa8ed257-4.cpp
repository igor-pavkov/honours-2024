TfLiteStatus GetAxisValueFromTensor(TfLiteContext* context,
                                    const TfLiteTensor& axis, int* axis_value) {
  TF_LITE_ENSURE_EQ(context, NumElements(&axis), 1);
  switch (axis.type) {
    case kTfLiteInt32:
      *axis_value = *GetTensorData<int32_t>(&axis);
      return kTfLiteOk;
    case kTfLiteInt64:
      *axis_value = *GetTensorData<int64_t>(&axis);
      return kTfLiteOk;
    default:
      return kTfLiteError;
  }
}