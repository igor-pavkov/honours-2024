TEST(QuantizedPoolingOpTest, SymmetricAveragePoolPaddingSameStride1) {
  // Choose the input ranges carefully so that the dequantized output matches
  // the results of the float model above.
  // Input Range[0, 15.9375] --> [Scale{0.0625}, zero_point{-128}]
  SymmetricQuantizedPoolingOpModel m(
      BuiltinOperator_AVERAGE_POOL_2D,
      /*input=*/{TensorType_INT8, {1, 2, 4, 1}, 0, 15.9375},
      /*filter_width=*/2, /*filter_height=*/2,
      /*output=*/{TensorType_INT8, {}, 0, 15.9375}, Padding_SAME, 1, 1);
  m.SetInput({
      0, 6, 2, 4,   //
      3, 2, 10, 7,  //
  });
  m.Invoke();

  EXPECT_THAT(m.GetDequantizedOutput(),
              ElementsAreArray(
                  ArrayFloatNear({2.75, 5.0, 5.75, 5.5, 2.5, 6.0, 8.5, 7.0})));
  EXPECT_THAT(m.GetOutput(),
              ElementsAreArray({44 - 128, 80 - 128, 92 - 128, 88 - 128,
                                40 - 128, 96 - 128, 136 - 128, 112 - 128}));
}