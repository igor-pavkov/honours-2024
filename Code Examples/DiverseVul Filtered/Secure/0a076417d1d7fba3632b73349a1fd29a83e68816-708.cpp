TEST(ExpressionObjectParse, ShouldRejectInvalidFieldName) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    ASSERT_THROWS(ExpressionObject::parse(expCtx, BSON("$a" << 1), vps), AssertionException);
    ASSERT_THROWS(ExpressionObject::parse(expCtx, BSON("" << 1), vps), AssertionException);
    ASSERT_THROWS(ExpressionObject::parse(expCtx, BSON(std::string("a\0b", 3) << 1), vps),
                  AssertionException);
}