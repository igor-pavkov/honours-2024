void CalculateUsedRange(const Tensor& input, qint32* used_min_quantized,
                        qint32* used_max_quantized) {
  auto input_array = input.flat<qint32>();
  Eigen::Tensor<qint32, 0, Eigen::RowMajor> min = input_array.minimum();
  Eigen::Tensor<qint32, 0, Eigen::RowMajor> max = input_array.maximum();
  *used_min_quantized = min();
  *used_max_quantized = max();
}