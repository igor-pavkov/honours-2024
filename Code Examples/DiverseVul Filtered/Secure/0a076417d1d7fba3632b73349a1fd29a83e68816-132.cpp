TEST(ParseExpression, ShouldParseExpressionWithMultipleArguments) {
    auto resultExpression = parseExpression(BSON("$strcasecmp" << BSON_ARRAY("foo"
                                                                             << "FOO")));
    auto strCaseCmpExpression = dynamic_cast<ExpressionStrcasecmp*>(resultExpression.get());
    ASSERT_TRUE(strCaseCmpExpression);
    vector<Value> arguments = {Value(Document{{"$const", "foo"_sd}}),
                               Value(Document{{"$const", "FOO"_sd}})};
    ASSERT_VALUE_EQ(strCaseCmpExpression->serialize(false),
                    Value(Document{{"$strcasecmp", arguments}}));
}