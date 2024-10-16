TEST(ParseExpression, ShouldAcceptArgumentWithoutArrayAsSingleArgument) {
    auto resultExpression = parseExpression(BSON("$not" << 1));
    auto notExpression = dynamic_cast<ExpressionNot*>(resultExpression.get());
    ASSERT_TRUE(notExpression);
    vector<Value> arguments = {Value(Document{{"$const", 1}})};
    ASSERT_VALUE_EQ(notExpression->serialize(false), Value(Document{{"$not", arguments}}));
}