TEST(ParseExpression, ShouldAcceptArgumentWithoutArrayForVariadicExpressions) {
    auto resultExpression = parseExpression(BSON("$and" << 1));
    auto andExpression = dynamic_cast<ExpressionAnd*>(resultExpression.get());
    ASSERT_TRUE(andExpression);
    vector<Value> arguments = {Value(Document{{"$const", 1}})};
    ASSERT_VALUE_EQ(andExpression->serialize(false), Value(Document{{"$and", arguments}}));
}