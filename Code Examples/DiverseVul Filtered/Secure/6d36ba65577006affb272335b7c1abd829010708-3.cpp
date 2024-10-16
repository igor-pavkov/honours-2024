TfLiteRegistration* Register_SPACE_TO_BATCH_ND_GENERIC_OPT() {
  static TfLiteRegistration r = {
      nullptr, nullptr, space_to_batch_nd::Prepare,
      space_to_batch_nd::Eval<space_to_batch_nd::kGenericOptimized>};
  return &r;
}