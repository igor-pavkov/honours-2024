TEST(QuantizedPoolingOpTest, MaxPoolLargeDepth) {
  // Test with a larger depth that is not a multiple of the tranche size, or of
  // any register-oriented multiples such as 8 and 16.
  constexpr int depth = 1999;  // Prime number.
  QuantizedPoolingOpModel m(
      BuiltinOperator_MAX_POOL_2D,
      /*input=*/{TensorType_UINT8, {1, 2, 4, depth}, 0, 15.9375},
      /*filter_width=*/2, /*filter_height=*/2,
      /*output=*/{TensorType_UINT8, {}, 0, 15.9375});

  std::vector<float> input_image_plane({
      0.f, 6.f, 2.f, 4.f,   //
      3.f, 2.f, 10.f, 7.f,  //
  });
  std::vector<float> output_image_plane({6.f, 10.f});

  m.SetInput(ReplicateDepthRamp(input_image_plane, depth, 1.f / 512.f));
  m.Invoke();

  EXPECT_THAT(m.GetDequantizedOutput(),
              ElementsAreArray(ArrayFloatNear(
                  ReplicateDepthRamp(output_image_plane, depth, 1.f / 512.f),
                  1. / 32.f)));
}