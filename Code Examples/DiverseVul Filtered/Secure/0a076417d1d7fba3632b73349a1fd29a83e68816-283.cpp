TEST(ExpressionTypeTest, WithRegexValue) {
    assertExpectedResults("$type", {{{Value(BSONRegEx("a.b"))}, Value("regex"_sd)}});
}