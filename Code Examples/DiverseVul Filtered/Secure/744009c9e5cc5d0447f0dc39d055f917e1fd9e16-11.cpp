void ScalarAddition(OpKernelContext* context, const T* full_input,
                    float full_input_min, float full_input_max,
                    int64 num_elements, T scalar_input, float scalar_input_min,
                    float scalar_input_max, float output_min, float output_max,
                    Toutput* output) {
  const Toutput scalar_in_output_range = RequantizeInNewRange<T, Toutput>(
      scalar_input, scalar_input_min, scalar_input_max, output_min, output_max);
  for (int i = 0; i < num_elements; ++i) {
    const Toutput full_input_in_output_range = RequantizeInNewRange<T, Toutput>(
        full_input[i], full_input_min, full_input_max, output_min, output_max);
    output[i] = full_input_in_output_range + scalar_in_output_range;
  }
}