TEST(ExpressionTypeTest, WithUndefinedValue) {
    assertExpectedResults("$type", {{{Value(BSONUndefined)}, Value("undefined"_sd)}});
}