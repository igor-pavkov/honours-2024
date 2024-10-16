TEST(ExpressionTypeTest, WithLongValue) {
    assertExpectedResults("$type", {{{Value(1LL)}, Value("long"_sd)}});
}