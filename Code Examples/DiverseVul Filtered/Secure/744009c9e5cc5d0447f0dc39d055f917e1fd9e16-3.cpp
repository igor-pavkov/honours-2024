void VectorTensorAddition(const T* vector_data, float min_vector,
                          float max_vector, int64 vector_num_elements,
                          const T* tensor_data, float min_tensor,
                          float max_tensor, int64 tensor_num_elements,
                          float output_min, float output_max, Toutput* output) {
  for (int i = 0; i < tensor_num_elements; ++i) {
    const int64 vector_i = i % vector_num_elements;
    const Toutput vector_in_output_range = RequantizeInNewRange<T, Toutput>(
        vector_data[vector_i], min_vector, max_vector, output_min, output_max);
    const Toutput tensor_in_output_range = RequantizeInNewRange<T, Toutput>(
        tensor_data[i], min_tensor, max_tensor, output_min, output_max);
    output[i] = vector_in_output_range + tensor_in_output_range;
  }
}