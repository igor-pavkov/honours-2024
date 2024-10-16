TfLiteRegistration* Register_MINIMUM_GENERIC_OPT() {
  static TfLiteRegistration r = {
      nullptr, nullptr, maximum_minimum::Prepare,
      maximum_minimum::Eval<maximum_minimum::kGenericOptimized,
                            maximum_minimum::MinimumOp>};
  return &r;
}