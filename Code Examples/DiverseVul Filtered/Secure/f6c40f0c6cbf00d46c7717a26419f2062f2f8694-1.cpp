inline InterpolationCache<T_SCALE> BuildLerpCache(
    const int64 out_size, const int64 in_size, const float scale,
    const int index_step, const int resolution, const bool half_pixel_centers) {
  InterpolationCache<T_SCALE> cache;
  // Compute the cached interpolation weights on the x and y dimensions.
  if (half_pixel_centers) {
    ComputeInterpolationWeights<T_SCALE, HalfPixelScaler>(
        out_size, in_size, scale, resolution, &cache);
  } else {
    ComputeInterpolationWeights<T_SCALE, LegacyScaler>(out_size, in_size, scale,
                                                       resolution, &cache);
  }
  CHECK(index_step > 0);
  if (index_step > 1) {
    for (int i = 0; i < cache.lower.size(); ++i) {
      cache.lower[i] *= index_step;
      cache.upper[i] *= index_step;
    }
  }
  return cache;
}