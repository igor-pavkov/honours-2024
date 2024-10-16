  TfLiteTensor* tensor(size_t index) override {
    return subgraph_->tensor(index);
  }