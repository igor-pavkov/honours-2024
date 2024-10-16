void VectorMultiply(OpKernelContext* context, const T* x_data, int32 offset_x,
                    const T* y_data, int32 offset_y, int64 num_elements,
                    Toutput* output) {
  for (int i = 0; i < num_elements; ++i) {
    output[i] = (static_cast<int32>(x_data[i]) - offset_x) *
                (static_cast<int32>(y_data[i]) - offset_y);
  }
}