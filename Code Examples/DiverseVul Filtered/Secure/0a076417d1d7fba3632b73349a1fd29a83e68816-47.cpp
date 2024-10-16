TEST(FieldPath, NoOptimizationOnNormalPath) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    intrusive_ptr<Expression> expression = ExpressionFieldPath::create(expCtx, "a");
    // An attempt to optimize returns the Expression itself.
    ASSERT_EQUALS(expression, expression->optimize());
}