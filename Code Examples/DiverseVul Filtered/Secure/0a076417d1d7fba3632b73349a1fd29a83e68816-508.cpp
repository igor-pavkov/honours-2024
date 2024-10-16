TEST(ExpressionTypeTest, WithMinKeyValue) {
    assertExpectedResults("$type", {{{Value(MINKEY)}, Value("minKey"_sd)}});
}