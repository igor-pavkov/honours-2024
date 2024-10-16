int ConvBackpropDimensions::SpatialPadding(const Padding& padding,
                                           int dim) const {
  return (padding == VALID)
             ? 0
             : std::max<int>(
                   0, static_cast<int>((output_size(dim) - 1) * stride(dim) +
                                       (filter_size(dim) - 1) * dilation(dim) +
                                       1 - input_size(dim)));
}