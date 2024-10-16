TEST(ParseExpression, ShouldRejectExpressionWithWrongNumberOfArguments) {
    ASSERT_THROWS(parseExpression(BSON("$strcasecmp" << BSON_ARRAY("foo"))), AssertionException);
}