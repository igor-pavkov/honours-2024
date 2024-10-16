TfLiteRegistration* Register_ONE_HOT() {
  static TfLiteRegistration r = {
      nullptr,
      nullptr,
      one_hot::Prepare,
      one_hot::Eval,
  };
  return &r;
}