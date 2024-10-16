TEST_F(ExpressionDateFromStringTest, RejectsTimeZoneInString) {
    auto expCtx = getExpCtx();

    auto spec = BSON("$dateFromString" << BSON("dateString"
                                               << "2017-07-13T10:02:57 Europe/London"));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_THROWS_CODE(dateExp->evaluate({}), AssertionException, 40553);

    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "July 4, 2017 Europe/London"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_THROWS_CODE(dateExp->evaluate({}), AssertionException, 40553);
}