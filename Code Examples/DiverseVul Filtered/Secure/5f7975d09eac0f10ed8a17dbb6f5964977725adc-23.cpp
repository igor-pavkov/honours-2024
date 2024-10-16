TEST(QuantizedInt8PoolingOpTest16, MaxPool) {
  // Choose the input ranges carefully so that the dequantized output matches
  // the results of the float model above.
  // Input Range[0, 16-(1/4096)] --> [Scale{(1/4096)}, zero_point{-32768}]
  const float ulp = 1.f / 4096.f;
  SymmetricQuantizedPoolingOpModel16 m(
      BuiltinOperator_MAX_POOL_2D,
      /*input=*/{TensorType_INT16, {1, 2, 4, 1}, 0, 16 - ulp},
      /*filter_width=*/2, /*filter_height=*/2,
      /*output=*/{TensorType_INT16, {}, 0, 16 - ulp});
  m.SetInput({
      0, 6, 2, 4,   //
      3, 2, 10, 7,  //
  });
  m.Invoke();

  EXPECT_THAT(m.GetDequantizedOutput(),
              ElementsAreArray(ArrayFloatNear({6, 10})));
  EXPECT_THAT(m.GetOutput(),
              ElementsAreArray({(96 - 128) * 256, (160 - 128) * 256}));
}