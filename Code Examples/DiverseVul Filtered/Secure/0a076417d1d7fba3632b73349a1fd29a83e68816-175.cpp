TEST(ParseExpression, ShouldRejectObjectWithTwoTopLevelExpressions) {
    ASSERT_THROWS(parseExpression(BSON("$and" << BSONArray() << "$or" << BSONArray())),
                  AssertionException);
}