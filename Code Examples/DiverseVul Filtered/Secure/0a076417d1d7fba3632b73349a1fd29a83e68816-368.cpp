TEST(ExpressionTypeTest, WithTimestampValue) {
    assertExpectedResults("$type", {{{Value(Timestamp(0, 0))}, Value("timestamp"_sd)}});
}