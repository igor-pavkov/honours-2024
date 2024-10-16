TEST(ExpressionTypeTest, WithBoolValue) {
    assertExpectedResults("$type", {{{Value(true)}, Value("bool"_sd)}});
}