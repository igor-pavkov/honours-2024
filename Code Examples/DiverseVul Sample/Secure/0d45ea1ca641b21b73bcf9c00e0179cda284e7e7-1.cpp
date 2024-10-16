TfLiteRegistration* Register_SPACE_TO_DEPTH_REF() {
  static TfLiteRegistration r = {
      nullptr, nullptr, space_to_depth::Prepare,
      space_to_depth::Eval<space_to_depth::kReference>};
  return &r;
}