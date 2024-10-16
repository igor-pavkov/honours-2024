TfLiteRegistration* Register_SPLIT() {
  static TfLiteRegistration r = {nullptr, nullptr, split::Prepare, split::Eval};
  return &r;
}