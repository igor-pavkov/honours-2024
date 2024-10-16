TEST(ExpressionTypeTest, WithOIDValue) {
    assertExpectedResults("$type", {{{Value(OID())}, Value("objectId"_sd)}});
}