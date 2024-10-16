TEST(ParseExpression, ShouldRejectExpressionArgumentsWhichAreNotInArray) {
    ASSERT_THROWS(parseExpression(BSON("$strcasecmp"
                                       << "foo")),
                  AssertionException);
}