TEST(GetComputedPathsTest, ExpressionFieldPathDoesNotCountAsRenameWhenDotted) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto expr = ExpressionFieldPath::parse(expCtx, "$a.b", expCtx->variablesParseState);
    auto computedPaths = expr->getComputedPaths("c", Variables::kRootId);
    ASSERT_EQ(computedPaths.paths.size(), 1u);
    ASSERT_EQ(computedPaths.paths.count("c"), 1u);
    ASSERT(computedPaths.renames.empty());
}