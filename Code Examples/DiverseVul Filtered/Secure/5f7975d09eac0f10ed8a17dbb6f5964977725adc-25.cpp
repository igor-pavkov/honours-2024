TEST(FloatPoolingOpTest, AveragePoolActivationRelu6) {
  FloatPoolingOpModel m(BuiltinOperator_AVERAGE_POOL_2D,
                        /*input=*/{TensorType_FLOAT32, {1, 2, 4, 1}},
                        /*filter_width=*/2, /*filter_height=*/2,
                        /*output=*/{TensorType_FLOAT32, {}}, Padding_VALID, 2,
                        2, ActivationFunctionType_RELU6);
  m.SetInput({
      0, -6, 12, 4,   //
      -3, -2, 10, 7,  //
  });
  m.Invoke();
  EXPECT_THAT(m.GetOutput(), ElementsAreArray({0.0, 6.0}));

  m.SetInput({
      0, 6, 12, 4,  //
      3, 2, 10, 7,  //
  });
  m.Invoke();
  EXPECT_THAT(m.GetOutput(), ElementsAreArray({2.75, 6.0}));
}