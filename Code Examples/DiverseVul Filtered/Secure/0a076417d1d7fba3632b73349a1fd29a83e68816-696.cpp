TEST(ExpressionTypeTest, WithStringValue) {
    assertExpectedResults("$type", {{{Value("stringValue"_sd)}, Value("string"_sd)}});
}