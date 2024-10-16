TEST(ExpressionTypeTest, WithDoubleValue) {
    assertExpectedResults("$type", {{{Value(1.0)}, Value("double"_sd)}});
}