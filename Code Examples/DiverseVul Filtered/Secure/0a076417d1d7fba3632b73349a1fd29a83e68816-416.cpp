TEST(GetComputedPathsTest, ExpressionFieldPathDoesNotCountAsRenameWithNonMatchingUserVariable) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    expCtx->variablesParseState.defineVariable("userVar");
    auto expr = ExpressionFieldPath::parse(expCtx, "$$userVar.b", expCtx->variablesParseState);
    auto computedPaths = expr->getComputedPaths("a", Variables::kRootId);
    ASSERT_EQ(computedPaths.paths.size(), 1u);
    ASSERT_EQ(computedPaths.paths.count("a"), 1u);
    ASSERT(computedPaths.renames.empty());
}