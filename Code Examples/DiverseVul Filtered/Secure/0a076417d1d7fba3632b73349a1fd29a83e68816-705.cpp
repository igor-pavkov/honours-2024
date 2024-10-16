TEST(FieldPath, OptimizeOnVariableWithConstantScalarValue) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto varId = expCtx->variablesParseState.defineVariable("userVar");
    expCtx->variables.setConstantValue(varId, Value(123));

    auto expr = ExpressionFieldPath::parse(expCtx, "$$userVar", expCtx->variablesParseState);
    ASSERT_TRUE(dynamic_cast<ExpressionFieldPath*>(expr.get()));

    auto optimizedExpr = expr->optimize();
    ASSERT_TRUE(dynamic_cast<ExpressionConstant*>(optimizedExpr.get()));
}