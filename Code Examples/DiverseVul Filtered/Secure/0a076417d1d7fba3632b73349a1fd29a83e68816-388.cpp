TEST(FieldPath, NoOptimizationOnVariableWithNoValue) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    expCtx->variablesParseState.defineVariable("userVar");

    auto expr = ExpressionFieldPath::parse(expCtx, "$$userVar", expCtx->variablesParseState);
    ASSERT_TRUE(dynamic_cast<ExpressionFieldPath*>(expr.get()));

    auto optimizedExpr = expr->optimize();
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(optimizedExpr.get()));
}