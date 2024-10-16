TfLiteRegistration* Register_MINIMUM_REF() {
  static TfLiteRegistration r = {
      nullptr, nullptr, maximum_minimum::Prepare,
      maximum_minimum::Eval<maximum_minimum::kReference,
                            maximum_minimum::MinimumOp>};
  return &r;
}