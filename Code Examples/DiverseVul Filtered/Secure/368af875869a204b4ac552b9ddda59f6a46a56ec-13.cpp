TEST(TensorSliceReaderTest, SimpleFloat) {
  SimpleFloatHelper(CreateTableTensorSliceBuilder, OpenTableTensorSliceReader);
}