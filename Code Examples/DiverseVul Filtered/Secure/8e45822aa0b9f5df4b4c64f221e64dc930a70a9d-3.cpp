TfLiteRegistration* Register_GATHER_ND() {
  static TfLiteRegistration r = {/*init*/ nullptr, /*free*/ nullptr,
                                 gather_nd::Prepare, gather_nd::Eval};
  return &r;
}