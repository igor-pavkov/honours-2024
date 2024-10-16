TEST(ExpressionTypeTest, WithNullValue) {
    assertExpectedResults("$type", {{{Value(BSONNULL)}, Value("null"_sd)}});
}