void VectorAddition(OpKernelContext* context, const T* x_data, float min_x,
                    float max_x, const T* y_data, float min_y, float max_y,
                    int64 num_elements, float output_min, float output_max,
                    Toutput* output) {
  for (int i = 0; i < num_elements; ++i) {
    const Toutput x_in_output_range = RequantizeInNewRange<T, Toutput>(
        x_data[i], min_x, max_x, output_min, output_max);
    const Toutput y_in_output_range = RequantizeInNewRange<T, Toutput>(
        y_data[i], min_y, max_y, output_min, output_max);
    output[i] = x_in_output_range + y_in_output_range;
  }
}