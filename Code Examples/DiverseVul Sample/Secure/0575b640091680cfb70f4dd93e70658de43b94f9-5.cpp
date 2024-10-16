void SparseLshProjection(const TfLiteTensor* hash, const TfLiteTensor* input,
                         const TfLiteTensor* weight, int32_t* out_buf) {
  int num_hash = SizeOfDimension(hash, 0);
  int num_bits = SizeOfDimension(hash, 1);
  for (int i = 0; i < num_hash; i++) {
    int32_t hash_signature = 0;
    for (int j = 0; j < num_bits; j++) {
      float seed = GetTensorData<float>(hash)[i * num_bits + j];
      int bit = RunningSignBit(input, weight, seed);
      hash_signature = (hash_signature << 1) | bit;
    }
    *out_buf++ = hash_signature + i * (1 << num_bits);
  }
}