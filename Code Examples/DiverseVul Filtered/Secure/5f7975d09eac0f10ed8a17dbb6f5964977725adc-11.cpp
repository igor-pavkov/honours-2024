  void SetInput(const std::vector<float>& data) {
    QuantizeAndPopulate<uint8_t>(input_, data);
  }