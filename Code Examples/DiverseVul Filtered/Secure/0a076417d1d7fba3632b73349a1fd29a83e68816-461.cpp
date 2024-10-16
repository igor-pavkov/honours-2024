TEST(GetComputedPathsTest, ExpressionObjectCorrectlyReportsComputedPathsNested) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto specObject = fromjson(
        "{a: {b: '$c'},"
        "d: {$map: {input: '$e', as: 'iter', in: {f: '$$iter.g'}}}}");
    auto expr = Expression::parseObject(expCtx, specObject, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionObject*>(expr.get()));
    auto computedPaths = expr->getComputedPaths("h");
    ASSERT(computedPaths.paths.empty());
    ASSERT_EQ(computedPaths.renames.size(), 2u);
    ASSERT_EQ(computedPaths.renames["h.a.b"], "c");
    ASSERT_EQ(computedPaths.renames["h.d.f"], "e.g");
}