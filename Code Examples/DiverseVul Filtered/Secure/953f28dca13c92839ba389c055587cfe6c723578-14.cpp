TfLiteRegistration* Register_MAXIMUM_GENERIC_OPT() {
  static TfLiteRegistration r = {
      nullptr, nullptr, maximum_minimum::Prepare,
      maximum_minimum::Eval<maximum_minimum::kGenericOptimized,
                            maximum_minimum::MaximumOp>};
  return &r;
}