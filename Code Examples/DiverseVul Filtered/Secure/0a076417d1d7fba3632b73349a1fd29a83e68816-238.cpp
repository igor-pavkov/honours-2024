TEST(ExpressionObjectParse, ShouldAcceptEmptyObject) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    auto object = ExpressionObject::parse(expCtx, BSONObj(), vps);
    ASSERT_VALUE_EQ(Value(Document{}), object->serialize(false));
}