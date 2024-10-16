void ResizeImageReference(typename TTypes<T, 4>::ConstTensor images,
                          const int batch_size, const int64 in_height,
                          const int64 in_width, const int64 out_height,
                          const int64 out_width, const int channels,
                          const float height_scale, const float width_scale,
                          const float in_min, const float in_max,
                          const bool half_pixel_centers,
                          typename TTypes<T, 4>::Tensor* output) {
  CHECK_NOTNULL(output);

  const InterpolationCache<float> xs = BuildLerpCache<float>(
      out_width, in_width, width_scale, channels, 0, half_pixel_centers);
  const InterpolationCache<float> ys = BuildLerpCache<float>(
      out_height, in_height, height_scale, 1, 0, half_pixel_centers);

  const int64 in_row_size = in_width * channels;
  const int64 in_batch_num_values = in_height * in_row_size;
  const int64 out_row_size = out_width * channels;

  const T* input_b_ptr = images.data();

  T* output_y_ptr = output->data();
  for (int b = 0; b < batch_size; ++b) {
    for (int64 y = 0; y < out_height; ++y) {
      const T* ys_input_lower_ptr = input_b_ptr + ys.lower[y] * in_row_size;
      const T* ys_input_upper_ptr = input_b_ptr + ys.upper[y] * in_row_size;
      const float ys_lerp = ys.lerp[y];
      for (int64 x = 0; x < out_width; ++x) {
        const int64 xs_lower = xs.lower[x];
        const int64 xs_upper = xs.upper[x];
        const float xs_lerp = xs.lerp[x];
        for (int c = 0; c < channels; ++c) {
          const T top_left = ys_input_lower_ptr[xs_lower + c];
          const T top_right = ys_input_lower_ptr[xs_upper + c];
          const T bottom_left = ys_input_upper_ptr[xs_lower + c];
          const T bottom_right = ys_input_upper_ptr[xs_upper + c];
          const T val = ComputeLerpReference<T>(
              top_left, top_right, bottom_left, bottom_right, xs_lerp, ys_lerp,
              in_min, in_max);
          output_y_ptr[x * channels + c] = val;
        }
      }
      output_y_ptr += out_row_size;
    }
    input_b_ptr += in_batch_num_values;
  }
}