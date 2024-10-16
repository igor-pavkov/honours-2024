std::vector<float> ReplicateDepthRamp(const std::vector<float>& image_plane,
                                      int n, float delta) {
  const int size = image_plane.size();
  std::vector<float> ramped_data(n * size);
  // The input is treated as a 1-D even if logically it is multi-dimensional.
  for (int input_index = 0; input_index < size; ++input_index) {
    for (int depth = 0; depth < n; ++depth) {
      ramped_data[n * input_index + depth] =
          image_plane[input_index] + depth * delta;
    }
  }

  return ramped_data;
}