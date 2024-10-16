TEST(ExpressionObjectDependencies, LocalLetVariablesShouldBeFilteredOutOfDependencies) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    expCtx->variablesParseState.defineVariable("var1");
    auto letSpec = BSON("$let" << BSON("vars" << BSON("var2"
                                                      << "abc")
                                              << "in"
                                              << BSON("$multiply" << BSON_ARRAY("$$var1"
                                                                                << "$$var2"))));
    auto expressionLet =
        ExpressionLet::parse(expCtx, letSpec.firstElement(), expCtx->variablesParseState);
    DepsTracker deps;
    expressionLet->addDependencies(&deps);
    ASSERT_EQ(deps.vars.size(), 1UL);
    ASSERT_EQ(expCtx->variablesParseState.getVariable("var1"), *deps.vars.begin());
}