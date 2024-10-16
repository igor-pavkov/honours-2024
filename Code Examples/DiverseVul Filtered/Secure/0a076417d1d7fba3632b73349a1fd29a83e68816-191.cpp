TEST(GetComputedPathsTest, ExpressionMapNotConsideredRenameWithWrongVariableNoExpressionObject) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto specObject = fromjson("{$map: {input: '$a', as: 'iter', in: '$b'}}");
    auto expr = Expression::parseObject(expCtx, specObject, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionMap*>(expr.get()));
    auto computedPaths = expr->getComputedPaths("d");
    ASSERT_EQ(computedPaths.paths.size(), 1u);
    ASSERT_EQ(computedPaths.paths.count("d"), 1u);
    ASSERT(computedPaths.renames.empty());
}