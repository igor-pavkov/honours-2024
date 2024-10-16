TfLiteRegistration* Register_SPACE_TO_DEPTH_GENERIC_OPT() {
  static TfLiteRegistration r = {
      nullptr, nullptr, space_to_depth::Prepare,
      space_to_depth::Eval<space_to_depth::kGenericOptimized>};
  return &r;
}