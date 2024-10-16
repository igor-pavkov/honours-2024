TEST(ExpressionTypeTest, WithSymbolValue) {
    assertExpectedResults("$type", {{{Value(BSONSymbol("a"))}, Value("symbol"_sd)}});
}