TEST(TensorSliceReaderTest, NegativeTensorShapeDimension) {
  const string fname =
      io::JoinPath(testing::TmpDir(), "negative_dim_checkpoint");
  TensorSliceWriter writer(fname, CreateTableTensorSliceBuilder);
  const int32 data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  TF_CHECK_OK(writer.Add("test", TensorShape({4, 5}),
                         TensorSlice::ParseOrDie("0,2:-"), data));
  TF_CHECK_OK(writer.Finish());

  MutateSavedTensorSlices(fname, [](SavedTensorSlices sts) {
    if (sts.has_meta()) {
      for (auto& tensor : *sts.mutable_meta()->mutable_tensor()) {
        for (auto& dim : *tensor.mutable_shape()->mutable_dim()) {
          dim.set_size(-dim.size());
        }
      }
    }
    return sts.SerializeAsString();
  });

  TensorSliceReader reader(fname, OpenTableTensorSliceReader);
  // The negative dimension should cause loading to fail.
  EXPECT_FALSE(reader.status().ok());
}