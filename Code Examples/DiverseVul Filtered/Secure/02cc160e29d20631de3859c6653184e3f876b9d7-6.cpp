  std::unique_ptr<IteratorBase> MakeIteratorInternal(
      const string& prefix) const override {
    return absl::make_unique<Iterator>(typename Iterator::Params{
        this, strings::StrCat(prefix, "::SparseTensorSlice")});
  }