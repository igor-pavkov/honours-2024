TEST(ExpressionTypeTest, WithMaxKeyValue) {
    assertExpectedResults("$type", {{{Value(MAXKEY)}, Value("maxKey"_sd)}});
}