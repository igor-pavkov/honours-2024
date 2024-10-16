void ResizeImage(typename TTypes<T, 4>::ConstTensor images,
                 const int batch_size, const int64 in_height,
                 const int64 in_width, const int64 out_height,
                 const int64 out_width, const int channels,
                 const float height_scale, const float width_scale,
                 const float in_min, const float in_max,
                 const bool half_pixel_centers,
                 typename TTypes<T, 4>::Tensor* output) {
  ResizeImageReference<T>(images, batch_size, in_height, in_width, out_height,
                          out_width, channels, height_scale, width_scale,
                          in_min, in_max, half_pixel_centers, output);
}