TEST(ParseExpression, ShouldRejectUnknownExpression) {
    ASSERT_THROWS(parseExpression(BSON("$invalid" << 1)), AssertionException);
}