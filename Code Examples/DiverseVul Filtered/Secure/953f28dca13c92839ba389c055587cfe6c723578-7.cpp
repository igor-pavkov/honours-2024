TfLiteRegistration* Register_MAXIMUM_REF() {
  static TfLiteRegistration r = {
      nullptr, nullptr, maximum_minimum::Prepare,
      maximum_minimum::Eval<maximum_minimum::kReference,
                            maximum_minimum::MaximumOp>};
  return &r;
}