TEST(FieldPath, RemoveOptimizesToMissingValue) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    intrusive_ptr<ExpressionFieldPath> expression =
        ExpressionFieldPath::parse(expCtx, "$$REMOVE", expCtx->variablesParseState);

    auto optimizedExpr = expression->optimize();

    ASSERT_VALUE_EQ(Value(), optimizedExpr->evaluate(Document(BSON("x" << BSON("y" << 123)))));
}