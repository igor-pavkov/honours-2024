TEST(ExpressionPowTest, LargeBaseSmallPositiveExponent) {
    assertExpectedResults("$pow",
                          {
                              {{Value(4294967296LL), Value(1LL)}, Value(4294967296LL)},
                              {{Value(4294967296LL), Value(0)}, Value(1LL)},
                          });
}