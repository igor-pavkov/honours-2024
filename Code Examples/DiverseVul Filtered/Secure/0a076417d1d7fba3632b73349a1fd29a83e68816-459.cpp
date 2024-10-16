TEST(ExpressionTypeTest, WithDBRefValue) {
    assertExpectedResults("$type", {{{Value(BSONDBRef("", OID()))}, Value("dbPointer"_sd)}});
}