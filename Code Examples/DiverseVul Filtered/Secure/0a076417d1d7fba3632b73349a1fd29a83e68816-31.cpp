TEST(ParseOperand, ShouldRecognizeNumberLiteral) {
    auto resultExpression = parseOperand(BSON("" << 5));
    auto constantExpression = dynamic_cast<ExpressionConstant*>(resultExpression.get());
    ASSERT_TRUE(constantExpression);
    ASSERT_VALUE_EQ(constantExpression->serialize(false), Value(Document{{"$const", 5}}));
}