TfLiteRegistration* Register_BATCH_TO_SPACE_ND_GENERIC_OPT() {
  static TfLiteRegistration r = {
      nullptr, nullptr, batch_to_space_nd::Prepare,
      batch_to_space_nd::Eval<batch_to_space_nd::kGenericOptimized>};
  return &r;
}