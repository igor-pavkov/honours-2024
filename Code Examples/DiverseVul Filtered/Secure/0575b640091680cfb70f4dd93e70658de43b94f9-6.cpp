TfLiteRegistration* Register_LSH_PROJECTION() {
  static TfLiteRegistration r = {nullptr, nullptr, lsh_projection::Resize,
                                 lsh_projection::Eval};
  return &r;
}