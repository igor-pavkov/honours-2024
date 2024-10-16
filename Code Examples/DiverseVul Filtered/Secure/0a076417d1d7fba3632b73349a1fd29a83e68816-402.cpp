TEST(ExpressionObjectParse, ShouldRejectDuplicateFieldNames) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    ASSERT_THROWS(ExpressionObject::parse(expCtx, BSON("a" << 1 << "a" << 1), vps),
                  AssertionException);
    ASSERT_THROWS(ExpressionObject::parse(expCtx, BSON("a" << 1 << "b" << 2 << "a" << 1), vps),
                  AssertionException);
    ASSERT_THROWS(
        ExpressionObject::parse(expCtx, BSON("a" << BSON("c" << 1) << "b" << 2 << "a" << 1), vps),
        AssertionException);
    ASSERT_THROWS(
        ExpressionObject::parse(expCtx, BSON("a" << 1 << "b" << 2 << "a" << BSON("c" << 1)), vps),
        AssertionException);
}