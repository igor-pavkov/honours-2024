static void VersionTest(const VersionDef& versions, const string& error) {
  const string path = io::JoinPath(testing::TmpDir(), "checkpoint");

  {
    // Prepare an empty checkpoint with some version information
    SavedTensorSlices sts;
    *sts.mutable_meta()->mutable_versions() = versions;
    string contents;
    EXPECT_TRUE(sts.SerializeToString(&contents));

    // Write it to disk
    TensorSliceWriter::Builder* builder;
    TF_ASSERT_OK(CreateTableTensorSliceBuilder(path, &builder));
    builder->Add(kSavedTensorSlicesKey, contents);
    int64_t file_size;
    TF_EXPECT_OK(builder->Finish(&file_size));
    delete builder;
  }

  // Read it back in and verify that we get the expected error
  TensorSliceReader reader(path, OpenTableTensorSliceReader);
  EXPECT_TRUE(reader.status().code() == error::INVALID_ARGUMENT &&
              absl::StartsWith(reader.status().error_message(), error))
      << "Expected error starting with '" << errors::InvalidArgument(error)
      << "', got '" << reader.status() << "'";
}