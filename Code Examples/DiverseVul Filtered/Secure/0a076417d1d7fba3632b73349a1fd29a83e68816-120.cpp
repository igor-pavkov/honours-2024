TEST_F(DateExpressionTest, RejectsUnrecognizedTimeZoneSpecification) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        BSONObj spec = BSON(expName << BSON("date" << Date_t{} << "timezone"
                                                   << "UNRECOGNIZED!"));
        auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        auto contextDoc = Document{{"_id", 0}};
        ASSERT_THROWS_CODE(dateExp->evaluate(contextDoc), AssertionException, 40485);
    }
}