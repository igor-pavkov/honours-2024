TEST(ExpressionTypeTest, WithDecimalValue) {
    assertExpectedResults("$type", {{{Value(Decimal128(0.3))}, Value("decimal"_sd)}});
}