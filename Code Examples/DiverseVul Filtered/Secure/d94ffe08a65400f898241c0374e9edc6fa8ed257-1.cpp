TfLiteRegistration* Register_EXPAND_DIMS() {
  static TfLiteRegistration r = {nullptr, nullptr, expand_dims::Prepare,
                                 expand_dims::Eval};
  return &r;
}