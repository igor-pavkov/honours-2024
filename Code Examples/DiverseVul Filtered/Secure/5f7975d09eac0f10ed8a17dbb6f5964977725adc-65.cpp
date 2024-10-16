TEST(QuantizedPoolingOpTest, AveragePoolImageSize17) {
  int image_size = 17;
  QuantizedPoolingOpModel m(
      BuiltinOperator_AVERAGE_POOL_2D,
      /*input=*/{TensorType_UINT8, {1, image_size, image_size, 1}, 0, 16},
      /*filter_width=*/image_size,
      /*filter_height=*/image_size,
      /*output=*/{TensorType_UINT8, {}, 0, 16});

  std::vector<float> input(image_size * image_size, 16.f);
  m.SetInput(input);
  m.Invoke();

  EXPECT_THAT(m.GetOutput(), ::testing::ElementsAre(255));
}