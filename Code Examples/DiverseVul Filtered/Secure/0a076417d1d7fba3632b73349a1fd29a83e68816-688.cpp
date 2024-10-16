TEST_F(DateExpressionTest, RejectsArraysWithinObjectSpecification) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        BSONObj spec = BSON(expName << BSON("date" << BSON_ARRAY(Date_t{}) << "timezone"
                                                   << "Europe/London"));
        // It will parse as an ExpressionArray, and fail at runtime.
        auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        auto contextDoc = Document{{"_id", 0}};
        ASSERT_THROWS_CODE(dateExp->evaluate(contextDoc), AssertionException, 16006);

        // Test that it rejects an array for the timezone option.
        spec =
            BSON(expName << BSON("date" << Date_t{} << "timezone" << BSON_ARRAY("Europe/London")));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        contextDoc = Document{{"_id", 0}};
        ASSERT_THROWS_CODE(dateExp->evaluate(contextDoc), AssertionException, 40533);
    }
}