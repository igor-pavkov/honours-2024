TEST(QuantizedPoolingOpTest, SymmetricAveragePool16) {
  const float ulp = 1.f / 4096.f;
  SymmetricQuantizedPoolingOpModel16 m(
      BuiltinOperator_AVERAGE_POOL_2D,
      /*input=*/{TensorType_INT16, {1, 2, 4, 1}, 0, 16 - ulp},
      /*filter_width=*/2, /*filter_height=*/2,
      /*output=*/{TensorType_INT16, {}, 0, 16 - ulp});
  m.SetInput({
      0, 6, 2, 4,   //
      3, 2, 10, 7,  //
  });
  m.Invoke();

  EXPECT_THAT(m.GetDequantizedOutput(),
              ElementsAreArray(ArrayFloatNear({2.75, 5.75})));
  EXPECT_THAT(m.GetOutput(),
              ElementsAreArray({(44 - 128) * 256, (92 - 128) * 256}));
}