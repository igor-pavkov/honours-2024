void ScalarMultiply(OpKernelContext* context, const T* full_input,
                    int32 full_input_offset, int64 num_elements, T scalar_input,
                    int32 scalar_input_offset, Toutput* output) {
  const int32 scalar_minus_offset =
      static_cast<int32>(scalar_input) - scalar_input_offset;
  for (int i = 0; i < num_elements; ++i) {
    output[i] = (static_cast<int32>(full_input[i]) - full_input_offset) *
                scalar_minus_offset;
  }
}