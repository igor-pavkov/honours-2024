  void SetInput(const std::vector<float>& data) {
    QuantizeAndPopulate<int16_t>(input_, data);
  }