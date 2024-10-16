TEST(ExpressionRangeTest, ComputesEmptyRange) {
    assertExpectedResults("$range",
                          {{{Value(-2), Value(3), Value(-1)}, Value(std::vector<Value>())}});
}