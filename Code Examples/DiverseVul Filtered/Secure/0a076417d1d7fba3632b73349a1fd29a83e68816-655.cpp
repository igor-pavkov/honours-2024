TEST(ExpressionObjectParse, ShouldRejectInvalidFieldPathAsValue) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    ASSERT_THROWS(ExpressionObject::parse(expCtx,
                                          BSON("a"
                                               << "$field."),
                                          vps),
                  AssertionException);
}