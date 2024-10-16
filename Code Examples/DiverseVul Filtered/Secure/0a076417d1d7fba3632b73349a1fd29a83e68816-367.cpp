TEST(ExpressionTypeTest, WithCodeValue) {
    assertExpectedResults("$type", {{{Value(BSONCode("var x = 3"))}, Value("javascript"_sd)}});
}