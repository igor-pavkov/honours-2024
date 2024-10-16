TEST(ExpressionObjectParse, ShouldAcceptFieldNameContainingDollar) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    auto object = ExpressionObject::parse(expCtx, BSON("a$b" << 5), vps);
    auto expectedResult = Value(Document{{"a$b", literal(5)}});
    ASSERT_VALUE_EQ(expectedResult, object->serialize(false));
}