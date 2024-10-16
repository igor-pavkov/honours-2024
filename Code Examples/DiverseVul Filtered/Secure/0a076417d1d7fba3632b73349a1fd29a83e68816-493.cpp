TEST(GetComputedPathsTest, ExpressionFieldPathDoesNotCountAsRenameWhenUsingRemoveBuiltin) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto expr = ExpressionFieldPath::parse(expCtx, "$$REMOVE", expCtx->variablesParseState);
    auto computedPaths = expr->getComputedPaths("a", Variables::kRootId);
    ASSERT_EQ(computedPaths.paths.size(), 1u);
    ASSERT_EQ(computedPaths.paths.count("a"), 1u);
    ASSERT(computedPaths.renames.empty());
}