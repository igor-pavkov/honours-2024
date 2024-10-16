TEST(ExpressionObjectDependencies, LocalFilterVariablesShouldBeFilteredOutOfDependencies) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    expCtx->variablesParseState.defineVariable("var1");
    auto filterSpec = BSON("$filter" << BSON("input" << BSON_ARRAY(1 << 2 << 3) << "as"
                                                     << "var2"
                                                     << "cond"
                                                     << BSON("$gt" << BSON_ARRAY("$$var1"
                                                                                 << "$$var2"))));

    auto expressionFilter =
        ExpressionFilter::parse(expCtx, filterSpec.firstElement(), expCtx->variablesParseState);
    DepsTracker deps;
    expressionFilter->addDependencies(&deps);
    ASSERT_EQ(deps.vars.size(), 1UL);
    ASSERT_EQ(expCtx->variablesParseState.getVariable("var1"), *deps.vars.begin());
}