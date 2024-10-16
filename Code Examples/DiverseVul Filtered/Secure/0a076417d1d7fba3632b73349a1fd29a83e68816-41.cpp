TEST(ExpressionSubstrCPTest, DoesThrowWithInvalidLeadingByte) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;

    const auto leadingByte = "\xFF\x00"_sd;
    const auto expr = Expression::parseExpression(
        expCtx, BSON("$substrCP" << BSON_ARRAY(leadingByte << 0 << 1)), vps);
    ASSERT_THROWS({ expr->evaluate(Document()); }, AssertionException);
}