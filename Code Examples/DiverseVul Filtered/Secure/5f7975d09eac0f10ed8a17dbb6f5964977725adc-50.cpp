TEST(FloatPoolingOpTest, AveragePoolActivationRelu1) {
  FloatPoolingOpModel m(BuiltinOperator_AVERAGE_POOL_2D,
                        /*input=*/{TensorType_FLOAT32, {1, 2, 4, 1}},
                        /*filter_width=*/2, /*filter_height=*/2,
                        /*output=*/{TensorType_FLOAT32, {}}, Padding_VALID, 2,
                        2, ActivationFunctionType_RELU_N1_TO_1);
  m.SetInput({
      0, -6, 2, 4,     //
      -3, -2, -10, 7,  //
  });
  m.Invoke();
  EXPECT_THAT(m.GetOutput(), ElementsAreArray({-1.0, 0.75}));

  m.SetInput({
      0, -6, -2, -4,   //
      -3, -2, 10, -7,  //
  });
  m.Invoke();
  EXPECT_THAT(m.GetOutput(), ElementsAreArray({-1.0, -0.75}));
}