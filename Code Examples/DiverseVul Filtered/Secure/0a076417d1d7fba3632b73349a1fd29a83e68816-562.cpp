TEST(GetComputedPathsTest, ExpressionMapCorrectlyReportsComputedPaths) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto specObject =
        fromjson("{$map: {input: '$a', as: 'iter', in: {b: '$$iter.c', d: {$add: [1, 2]}}}}");
    auto expr = Expression::parseObject(expCtx, specObject, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionMap*>(expr.get()));
    auto computedPaths = expr->getComputedPaths("e");
    ASSERT_EQ(computedPaths.paths.size(), 1u);
    ASSERT_EQ(computedPaths.paths.count("e.d"), 1u);
    ASSERT_EQ(computedPaths.renames.size(), 1u);
    ASSERT_EQ(computedPaths.renames["e.b"], "a.c");
}