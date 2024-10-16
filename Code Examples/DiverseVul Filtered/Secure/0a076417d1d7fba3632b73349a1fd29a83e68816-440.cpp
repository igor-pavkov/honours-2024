TEST(GetComputedPathsTest, ExpressionMapNotConsideredRenameWithDottedInputPath) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto specObject = fromjson("{$map: {input: '$a.b', as: 'iter', in: {c: '$$iter.d'}}}}");
    auto expr = Expression::parseObject(expCtx, specObject, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionMap*>(expr.get()));
    auto computedPaths = expr->getComputedPaths("e");
    ASSERT_EQ(computedPaths.paths.size(), 1u);
    ASSERT_EQ(computedPaths.paths.count("e"), 1u);
    ASSERT(computedPaths.renames.empty());
}