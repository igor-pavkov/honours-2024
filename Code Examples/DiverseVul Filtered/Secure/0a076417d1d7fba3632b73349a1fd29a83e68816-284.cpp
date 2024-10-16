TEST(ExpressionObjectDependencies, VariablesShouldBeAddedToDependencies) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto varID = expCtx->variablesParseState.defineVariable("var1");
    auto fieldPath = ExpressionFieldPath::parse(expCtx, "$$var1", expCtx->variablesParseState);
    DepsTracker deps;
    fieldPath->addDependencies(&deps);
    ASSERT_EQ(deps.vars.size(), 1UL);
    ASSERT_EQ(deps.vars.count(varID), 1UL);
}