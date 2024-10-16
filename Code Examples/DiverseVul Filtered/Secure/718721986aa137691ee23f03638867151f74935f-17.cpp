TfLiteRegistration* Register_FULLY_CONNECTED_PIE() {
  static TfLiteRegistration r = {
      fully_connected::Init, fully_connected::Free,
      fully_connected::Prepare<fully_connected::kLegacyPie>,
      fully_connected::Eval<fully_connected::kLegacyPie>};
  return &r;
}