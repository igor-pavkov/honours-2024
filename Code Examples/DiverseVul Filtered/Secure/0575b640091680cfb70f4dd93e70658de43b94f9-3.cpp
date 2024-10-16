int RunningSignBit(const TfLiteTensor* input, const TfLiteTensor* weight,
                   float seed) {
  double score = 0.0;
  int input_item_bytes = input->bytes / SizeOfDimension(input, 0);
  char* input_ptr = input->data.raw;

  const size_t seed_size = sizeof(float);
  const size_t key_bytes = sizeof(float) + input_item_bytes;
  std::unique_ptr<char[]> key(new char[key_bytes]);

  const float* weight_ptr = GetTensorData<float>(weight);

  for (int i = 0; i < SizeOfDimension(input, 0); ++i) {
    // Create running hash id and value for current dimension.
    memcpy(key.get(), &seed, seed_size);
    memcpy(key.get() + seed_size, input_ptr, input_item_bytes);

    int64_t hash_signature = ::util::Fingerprint64(key.get(), key_bytes);
    double running_value = static_cast<double>(hash_signature);
    input_ptr += input_item_bytes;
    if (weight_ptr == nullptr) {
      score += running_value;
    } else {
      score += weight_ptr[i] * running_value;
    }
  }

  return (score > 0) ? 1 : 0;
}