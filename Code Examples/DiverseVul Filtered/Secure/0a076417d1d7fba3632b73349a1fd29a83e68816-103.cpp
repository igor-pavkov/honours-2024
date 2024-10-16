TEST(GetComputedPathsTest, ExpressionMapCorrectlyReportsComputedPathsWithNestedExprObject) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto specObject = fromjson("{$map: {input: '$a', in: {b: {c: '$$this.d'}}}}");
    auto expr = Expression::parseObject(expCtx, specObject, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionMap*>(expr.get()));
    auto computedPaths = expr->getComputedPaths("e");
    ASSERT(computedPaths.paths.empty());
    ASSERT_EQ(computedPaths.renames.size(), 1u);
    ASSERT_EQ(computedPaths.renames["e.b.c"], "a.d");
}