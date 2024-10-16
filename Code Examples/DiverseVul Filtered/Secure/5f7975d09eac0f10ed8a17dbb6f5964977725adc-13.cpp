TEST(QuantizedUInt8PoolingOpTest, MaxPoolActivationRelu) {
  // Choose the input ranges carefully so that the dequantized output matches
  // the results of the float model above.
  // Input Range[-15.9375, 15.9375] --> [Scale{0.125}, zero_point{128}]
  QuantizedPoolingOpModel m(
      BuiltinOperator_MAX_POOL_2D,
      /*input=*/{TensorType_UINT8, {1, 2, 4, 1}, -15.9375, 15.9375},
      /*filter_width=*/2, /*filter_height=*/2,
      /*output=*/{TensorType_UINT8, {}, -15.9375, 15.9375}, Padding_VALID, 2, 2,
      ActivationFunctionType_RELU);
  m.SetInput({
      -1.5, -6, 2, 4,  //
      -3, -2, 10, 7,   //
  });
  m.Invoke();

  EXPECT_THAT(m.GetDequantizedOutput(),
              ElementsAreArray(ArrayFloatNear({0, 10})));
  EXPECT_THAT(m.GetOutput(), ElementsAreArray({128, 208}));
}