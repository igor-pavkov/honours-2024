TEST(GetComputedPathsTest, ExpressionFieldPathDoesCountAsRenameWithExplicitRoot) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto expr = ExpressionFieldPath::parse(expCtx, "$$ROOT.a", expCtx->variablesParseState);
    auto computedPaths = expr->getComputedPaths("b", Variables::kRootId);
    ASSERT(computedPaths.paths.empty());
    ASSERT_EQ(computedPaths.renames.size(), 1u);
    ASSERT_EQ(computedPaths.renames["b"], "a");
}