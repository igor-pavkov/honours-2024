TEST_F(DateExpressionTest, RejectsEmptyArray) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        BSONObj spec = BSON(expName << BSONArray());
        // It will parse as an ExpressionArray, and fail at runtime.
        ASSERT_THROWS_CODE(Expression::parseExpression(expCtx, spec, expCtx->variablesParseState),
                           AssertionException,
                           40536);
    }
}