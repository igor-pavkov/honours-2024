TEST(ParseOperand, ShouldRecognizeNestedArray) {
    auto resultExpression = parseOperand(BSON("" << BSON_ARRAY("foo"
                                                               << "$field")));
    auto arrayExpression = dynamic_cast<ExpressionArray*>(resultExpression.get());
    ASSERT_TRUE(arrayExpression);
    vector<Value> expectedSerializedArray = {Value(Document{{"$const", "foo"_sd}}),
                                             Value("$field"_sd)};
    ASSERT_VALUE_EQ(arrayExpression->serialize(false), Value(expectedSerializedArray));
}