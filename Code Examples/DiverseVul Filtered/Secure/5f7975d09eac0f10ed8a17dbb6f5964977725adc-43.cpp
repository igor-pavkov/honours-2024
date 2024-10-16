TEST(FloatPoolingOpTest, L2PoolPaddingSameSlide1) {
  FloatPoolingOpModel m(BuiltinOperator_L2_POOL_2D,
                        /*input=*/{TensorType_FLOAT32, {1, 2, 4, 1}},
                        /*filter_width=*/2, /*filter_height=*/2,
                        /*output=*/{TensorType_FLOAT32, {}}, Padding_SAME, 1,
                        1);
  m.SetInput({
      0, 6, 2, 4,   //
      3, 2, 10, 7,  //
  });
  m.Invoke();
  EXPECT_THAT(m.GetOutput(),
              ElementsAreArray(ArrayFloatNear(
                  {3.5, 6.0, 6.5, 5.70088, 2.54951, 7.2111, 8.63134, 7.0},
                  /*max_abs_error=*/1e-4)));
}