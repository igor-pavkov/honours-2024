TEST(ExpressionObjectDependencies, LocalMapVariablesShouldBeFilteredOutOfDependencies) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    expCtx->variablesParseState.defineVariable("var1");
    auto mapSpec = BSON("$map" << BSON("input"
                                       << "$field1"
                                       << "as"
                                       << "var2"
                                       << "in"
                                       << BSON("$multiply" << BSON_ARRAY("$$var1"
                                                                         << "$$var2"))));

    auto expressionMap =
        ExpressionMap::parse(expCtx, mapSpec.firstElement(), expCtx->variablesParseState);
    DepsTracker deps;
    expressionMap->addDependencies(&deps);
    ASSERT_EQ(deps.vars.size(), 1UL);
    ASSERT_EQ(expCtx->variablesParseState.getVariable("var1"), *deps.vars.begin());
}