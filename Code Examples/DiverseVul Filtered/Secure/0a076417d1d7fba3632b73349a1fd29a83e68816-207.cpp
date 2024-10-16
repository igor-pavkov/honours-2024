TEST(ExpressionRangeTest, ComputesRangeWithSameStartAndEnd) {
    assertExpectedResults("$range", {{{Value(20), Value(20)}, Value(std::vector<Value>())}});
}