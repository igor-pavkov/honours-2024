TEST(GetComputedPathsTest, ExpressionFieldPathDoesNotCountAsRenameWhenOnlyRoot) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto expr = ExpressionFieldPath::parse(expCtx, "$$ROOT", expCtx->variablesParseState);
    auto computedPaths = expr->getComputedPaths("a", Variables::kRootId);
    ASSERT_EQ(computedPaths.paths.size(), 1u);
    ASSERT_EQ(computedPaths.paths.count("a"), 1u);
    ASSERT(computedPaths.renames.empty());
}