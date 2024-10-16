TEST(FieldPath, NoOptimizationForCurrentFieldPathWithDottedPath) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    intrusive_ptr<ExpressionFieldPath> expression =
        ExpressionFieldPath::parse(expCtx, "$$CURRENT.x.y", expCtx->variablesParseState);

    // An attempt to optimize returns the Expression itself.
    ASSERT_EQUALS(expression, expression->optimize());
}