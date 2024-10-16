void Tensor::CheckIsAlignedAndSingleElement() const {
  CHECK(IsAligned()) << "Aligned and single element";
  CHECK_EQ(1, NumElements()) << "Must have a one element tensor";
}