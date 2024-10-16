TEST(GetComputedPathsTest, ExpressionObjectCorrectlyReportsComputedPaths) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto specObject = fromjson("{a: '$b', c: {$add: [1, 3]}}");
    auto expr = Expression::parseObject(expCtx, specObject, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionObject*>(expr.get()));
    auto computedPaths = expr->getComputedPaths("d");
    ASSERT_EQ(computedPaths.paths.size(), 1u);
    ASSERT_EQ(computedPaths.paths.count("d.c"), 1u);
    ASSERT_EQ(computedPaths.renames.size(), 1u);
    ASSERT_EQ(computedPaths.renames["d.a"], "b");
}