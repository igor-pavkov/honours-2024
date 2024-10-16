  void SetInput(const std::vector<float>& data) {
    QuantizeAndPopulate<int8_t>(input_, data);
  }