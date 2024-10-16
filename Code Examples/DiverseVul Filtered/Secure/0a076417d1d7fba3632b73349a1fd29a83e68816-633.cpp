TEST(ExpressionObjectParse, ShouldRejectDottedFieldNames) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    ASSERT_THROWS(ExpressionObject::parse(expCtx, BSON("a.b" << 1), vps), AssertionException);
    ASSERT_THROWS(ExpressionObject::parse(expCtx, BSON("c" << 3 << "a.b" << 1), vps),
                  AssertionException);
    ASSERT_THROWS(ExpressionObject::parse(expCtx, BSON("a.b" << 1 << "c" << 3), vps),
                  AssertionException);
}