TfLiteRegistration* Register_SPACE_TO_BATCH_ND_REF() {
  static TfLiteRegistration r = {
      nullptr, nullptr, space_to_batch_nd::Prepare,
      space_to_batch_nd::Eval<space_to_batch_nd::kReference>};
  return &r;
}