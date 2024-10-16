TEST(FieldPath, NoOptimizationOnVariableWithMissingValue) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto varId = expCtx->variablesParseState.defineVariable("userVar");
    expCtx->variables.setValue(varId, Value());

    auto expr = ExpressionFieldPath::parse(expCtx, "$$userVar", expCtx->variablesParseState);
    ASSERT_TRUE(dynamic_cast<ExpressionFieldPath*>(expr.get()));

    auto optimizedExpr = expr->optimize();
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(optimizedExpr.get()));
}