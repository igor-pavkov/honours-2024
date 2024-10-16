TEST(ExpressionTypeTest, WithIntValue) {
    assertExpectedResults("$type", {{{Value(1)}, Value("int"_sd)}});
}