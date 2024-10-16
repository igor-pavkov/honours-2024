void ResizeBilinear(const typename TTypes<T, 4>::ConstTensor& images,
                    const float height_scale, const float width_scale,
                    const float in_min, const float in_max,
                    const bool half_pixel_centers,
                    typename TTypes<T, 4>::Tensor* output) {
  CHECK_NOTNULL(output);

  const int batch_size = images.dimension(0);
  const int64 in_height = images.dimension(1);
  const int64 in_width = images.dimension(2);
  const int channels = images.dimension(3);

  const int64 out_height = output->dimension(1);
  const int64 out_width = output->dimension(2);

  // Handle no-op resizes efficiently.
  if (out_height == in_height && out_width == in_width) {
    *output = images.template cast<T>();
    return;
  }

  if (USE_REFERENCE) {
    ResizeImageReference<T>(images, batch_size, in_height, in_width, out_height,
                            out_width, channels, height_scale, width_scale,
                            in_min, in_max, half_pixel_centers, output);
  } else {
    ResizeImage<T>(images, batch_size, in_height, in_width, out_height,
                   out_width, channels, height_scale, width_scale, in_min,
                   in_max, half_pixel_centers, output);
  }
}