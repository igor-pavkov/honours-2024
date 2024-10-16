TEST(ExpressionObjectParse, ShouldAccept_idAsFieldName) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    auto object = ExpressionObject::parse(expCtx, BSON("_id" << 5), vps);
    auto expectedResult = Value(Document{{"_id", literal(5)}});
    ASSERT_VALUE_EQ(expectedResult, object->serialize(false));
}