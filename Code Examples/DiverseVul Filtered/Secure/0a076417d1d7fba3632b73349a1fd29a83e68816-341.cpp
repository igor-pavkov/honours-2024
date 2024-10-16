TEST(ExpressionSubstrCPTest, WithPartiallyOutOfRangeString) {
    assertExpectedResults("$substrCP", {{{Value("abc"_sd), Value(1), Value(4)}, Value("bc"_sd)}});
}