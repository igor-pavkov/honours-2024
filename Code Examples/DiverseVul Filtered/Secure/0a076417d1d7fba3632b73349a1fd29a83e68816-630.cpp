TEST_F(ExpressionDateFromStringTest, RejectsTimeZoneInStringAndArgument) {
    auto expCtx = getExpCtx();

    // Test with "Z" and timezone
    auto spec = BSON("$dateFromString" << BSON("dateString"
                                               << "2017-07-14T15:24:38Z"
                                               << "timezone"
                                               << "Europe/London"));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_THROWS_CODE(dateExp->evaluate({}), AssertionException, 40551);

    // Test with timezone abbreviation and timezone
    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "2017-07-14T15:24:38 PDT"
                                          << "timezone"
                                          << "Europe/London"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_THROWS_CODE(dateExp->evaluate({}), AssertionException, 40551);

    // Test with GMT offset and timezone
    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "2017-07-14T15:24:38+02:00"
                                          << "timezone"
                                          << "Europe/London"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_THROWS_CODE(dateExp->evaluate({}), AssertionException, 40554);

    // Test with GMT offset and GMT timezone
    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "2017-07-14 -0400"
                                          << "timezone"
                                          << "GMT"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_THROWS_CODE(dateExp->evaluate({}), AssertionException, 40554);
}