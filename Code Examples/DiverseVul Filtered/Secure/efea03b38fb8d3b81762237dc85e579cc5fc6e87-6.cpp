void VectorTensorMultiply(const T* vector_data, int32 vector_offset,
                          int64 vector_num_elements, const T* tensor_data,
                          int32 tensor_offset, int64 tensor_num_elements,
                          Toutput* output) {
  for (int i = 0; i < tensor_num_elements; ++i) {
    const int64 vector_i = i % vector_num_elements;
    output[i] = (static_cast<int32>(vector_data[vector_i]) - vector_offset) *
                (static_cast<int32>(tensor_data[i]) - tensor_offset);
  }
}