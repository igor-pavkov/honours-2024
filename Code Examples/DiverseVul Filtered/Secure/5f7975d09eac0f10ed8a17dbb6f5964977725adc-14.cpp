TEST(FloatPoolingOpTest, L2PoolActivationRelu1) {
  FloatPoolingOpModel m(BuiltinOperator_L2_POOL_2D,
                        /*input=*/{TensorType_FLOAT32, {1, 2, 4, 1}},
                        /*filter_width=*/2, /*filter_height=*/2,
                        /*output=*/{TensorType_FLOAT32, {}}, Padding_VALID, 2,
                        2, ActivationFunctionType_RELU_N1_TO_1);
  m.SetInput({
      -0.1, -0.6, 2, 4,   //
      -0.3, -0.2, 10, 7,  //
  });
  m.Invoke();
  EXPECT_THAT(m.GetOutput(), ElementsAreArray(ArrayFloatNear({0.353553, 1.0})));
}