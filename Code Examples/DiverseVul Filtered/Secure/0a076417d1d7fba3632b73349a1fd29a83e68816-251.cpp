TEST_F(DateExpressionTest, RejectsNonStringTimezone) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        BSONObj spec = BSON(expName << BSON("date" << Date_t{} << "timezone"
                                                   << "$intField"));
        auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        auto contextDoc = Document{{"intField", 4}};
        ASSERT_THROWS_CODE(dateExp->evaluate(contextDoc), AssertionException, 40533);
    }
}