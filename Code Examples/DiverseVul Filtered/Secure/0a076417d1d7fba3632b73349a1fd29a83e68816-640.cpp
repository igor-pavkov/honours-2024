TEST(ParseObject, ShouldRejectExpressionAsTheSecondField) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    ASSERT_THROWS(
        ExpressionObject::parse(
            expCtx, BSON("a" << BSON("$and" << BSONArray()) << "$or" << BSONArray()), vps),
        AssertionException);
}