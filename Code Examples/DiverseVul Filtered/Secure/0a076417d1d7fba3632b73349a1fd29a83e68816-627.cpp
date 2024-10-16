TEST(FieldPath, ScalarVariableWithDottedFieldPathOptimizesToConstantMissingValue) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto varId = expCtx->variablesParseState.defineVariable("userVar");
    expCtx->variables.setConstantValue(varId, Value(123));

    auto expr = ExpressionFieldPath::parse(expCtx, "$$userVar.x.y", expCtx->variablesParseState);
    ASSERT_TRUE(dynamic_cast<ExpressionFieldPath*>(expr.get()));

    auto optimizedExpr = expr->optimize();
    auto constantExpr = dynamic_cast<ExpressionConstant*>(optimizedExpr.get());
    ASSERT_TRUE(constantExpr);
    ASSERT_VALUE_EQ(Value(), constantExpr->getValue());
}