void SimpleFloatHelper(
    const TensorSliceWriter::CreateBuilderFunction& create_function,
    TensorSliceReader::OpenTableFunction open_function) {
  const string fname_base = io::JoinPath(testing::TmpDir(), "float_checkpoint");

  TensorShape shape({4, 5});

  // File #0 contains a slice that is the top two rows:
  //
  //   0   1   2   3   4
  //   5   6   7   8   9
  //   .   .   .   .   .
  //   .   .   .   .   .
  {
    const string fname = strings::StrCat(fname_base, "_0");
    TensorSliceWriter writer(fname, create_function);
    const float data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    TensorSlice slice = TensorSlice::ParseOrDie("0,2:-");
    TF_CHECK_OK(writer.Add("test", shape, slice, data));
    TF_CHECK_OK(writer.Finish());
  }

  // File #1 contains two slices:
  //
  // slice #0 is the bottom left corner
  //   .   .   .   .   .
  //   .   .   .   .   .
  //  10  11  12   .   .
  //  15  16  17   .   .
  //
  // slice #1 is the bottom right corner
  //   .   .   .   .   .
  //   .   .   .   .   .
  //   .   .   .   .   .
  //   .   .   .  18  19
  {
    const string fname = strings::StrCat(fname_base, "_1");
    TensorSliceWriter writer(fname, create_function);
    // slice #0
    {
      const float data[] = {10, 11, 12, 15, 16, 17};
      TensorSlice slice = TensorSlice::ParseOrDie("2,2:0,3");
      TF_CHECK_OK(writer.Add("test", shape, slice, data));
    }
    // slice #1
    {
      const float data[] = {18, 19};
      TensorSlice slice = TensorSlice::ParseOrDie("3,1:3,2");
      TF_CHECK_OK(writer.Add("test", shape, slice, data));
    }
    TF_CHECK_OK(writer.Finish());
  }

  // Notice that we leave a hole in the tensor
  //   .   .   .   .   .
  //   .   .   .   .   .
  //   .   .   . (13) (14)
  //   .   .   .   .   .

  // Now we need to read the tensor slices
  const string filepattern = strings::StrCat(fname_base, "_*");
  TensorSliceReader reader(filepattern, std::move(open_function));
  TF_EXPECT_OK(reader.status());
  EXPECT_EQ(2, reader.num_files());

  // We query some of the tensors
  {
    TensorShape shape;
    DataType type;
    EXPECT_TRUE(reader.HasTensor("test", &shape, &type));
    EXPECT_EQ("[4,5]", shape.DebugString());
    EXPECT_EQ(DT_FLOAT, type);
    EXPECT_FALSE(reader.HasTensor("don't exist", nullptr, nullptr));
  }

  // Now we query some slices
  //
  // Slice #1 is an exact match
  //   0   1   2   3   4
  //   5   6   7   8   9
  //   .   .   .   .   .
  //   .   .   .   .   .
  {
    TensorSlice s = TensorSlice::ParseOrDie("0,2:-");
    float expected[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    float results[10];
    EXPECT_TRUE(reader.CopySliceData("test", s, results));
    for (int i = 0; i < 10; ++i) {
      EXPECT_EQ(expected[i], results[i]);
    }
  }

  // Slice #2 is a subset match
  //   .   .   .   .   .
  //   5   6   7   8   9
  //   .   .   .   .   .
  //   .   .   .   .   .
  {
    TensorSlice s = TensorSlice::ParseOrDie("1,1:-");
    float expected[] = {5, 6, 7, 8, 9};
    float results[5];
    EXPECT_TRUE(reader.CopySliceData("test", s, results));
    for (int i = 0; i < 5; ++i) {
      EXPECT_EQ(expected[i], results[i]);
    }
  }

  // Slice #4 includes the hole and so there is no match
  //   .   .   .   .   .
  //   .   .   7   8   9
  //   .   .  12  13  14
  //   .   .   .   .   .
  {
    TensorSlice s = TensorSlice::ParseOrDie("1,2:2,3");
    float results[6];
    EXPECT_FALSE(reader.CopySliceData("test", s, results));
  }
}