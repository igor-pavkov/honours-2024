TfLiteStatus GatherNdString(const TfLiteTensor* params,
                            const TfLiteTensor* indices, TfLiteTensor* output) {
  reference_ops::GatherNdString(
      GetTensorShape(params), params, GetTensorShape(indices),
      GetTensorData<IndicesT>(indices), GetTensorShape(output), output);
  return kTfLiteOk;
}