  static bool SplitHasAlignedOutputsInFirstDimension(
      const TensorShape& input_shape, int32_t split_dim,
      absl::Span<const Tlen> split_sizes) {
    if (split_dim != 0) {
      return false;
    }
    Tlen start = 0;
    for (const Tlen split_size : split_sizes) {
      if (!IsDim0SliceAligned<T>(input_shape, start, start + split_size)) {
        return false;
      }
      start += split_size;
    }
    return true;
  }