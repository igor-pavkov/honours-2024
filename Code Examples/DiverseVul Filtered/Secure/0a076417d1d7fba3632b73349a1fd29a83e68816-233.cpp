TEST(ExpressionPowTest, ThrowsWhenBaseZeroAndExpNegative) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;

    const auto expr = Expression::parseExpression(expCtx, BSON("$pow" << BSON_ARRAY(0 << -5)), vps);
    ASSERT_THROWS([&] { expr->evaluate(Document()); }(), AssertionException);

    const auto exprWithLong =
        Expression::parseExpression(expCtx, BSON("$pow" << BSON_ARRAY(0LL << -5LL)), vps);
    ASSERT_THROWS([&] { expr->evaluate(Document()); }(), AssertionException);
}