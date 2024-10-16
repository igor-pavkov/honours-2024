TEST(ExpressionTypeTest, WithArrayValue) {
    assertExpectedResults("$type", {{{Value(BSON_ARRAY(1 << 2))}, Value("array"_sd)}});
}