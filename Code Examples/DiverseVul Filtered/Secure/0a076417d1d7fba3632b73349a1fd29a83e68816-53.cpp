TEST(ParseOperand, ShouldRecognizeFieldPath) {
    auto resultExpression = parseOperand(BSON(""
                                              << "$field"));
    auto fieldPathExpression = dynamic_cast<ExpressionFieldPath*>(resultExpression.get());
    ASSERT_TRUE(fieldPathExpression);
    ASSERT_VALUE_EQ(fieldPathExpression->serialize(false), Value("$field"_sd));
}