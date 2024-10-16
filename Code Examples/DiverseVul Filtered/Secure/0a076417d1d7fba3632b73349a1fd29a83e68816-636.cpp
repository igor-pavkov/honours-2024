TEST(ExpressionSubstrCPTest, DoesThrowWithBadContinuationByte) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;

    const auto continuationByte = "\x80\x00"_sd;
    const auto expr = Expression::parseExpression(
        expCtx, BSON("$substrCP" << BSON_ARRAY(continuationByte << 0 << 1)), vps);
    ASSERT_THROWS({ expr->evaluate(Document()); }, AssertionException);
}