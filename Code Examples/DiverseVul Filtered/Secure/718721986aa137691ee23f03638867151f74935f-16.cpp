TfLiteRegistration* Register_FULLY_CONNECTED_GENERIC_OPT() {
  static TfLiteRegistration r = {
      fully_connected::Init, fully_connected::Free,
      fully_connected::Prepare<fully_connected::kGenericOptimized>,
      fully_connected::Eval<fully_connected::kGenericOptimized>};
  return &r;
}