TEST(ExpressionSubstrCPTest, WithStandardValue) {
    assertExpectedResults("$substrCP", {{{Value("abc"_sd), Value(0), Value(2)}, Value("ab"_sd)}});
}