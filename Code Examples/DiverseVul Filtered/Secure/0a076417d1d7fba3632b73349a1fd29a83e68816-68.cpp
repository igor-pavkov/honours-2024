TEST(ParseExpression, ShouldParseExpressionWithNoArguments) {
    auto resultExpression = parseExpression(BSON("$and" << BSONArray()));
    auto andExpression = dynamic_cast<ExpressionAnd*>(resultExpression.get());
    ASSERT_TRUE(andExpression);
    ASSERT_VALUE_EQ(andExpression->serialize(false), Value(Document{{"$and", vector<Value>{}}}));
}