  void SetInput(std::initializer_list<float> data) {
    QuantizeAndPopulate<int16_t>(input_, data);
  }