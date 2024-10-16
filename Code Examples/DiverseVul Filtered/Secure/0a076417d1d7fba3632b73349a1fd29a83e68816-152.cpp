TEST_F(ExpressionDateFromStringTest, ReadWithUTCOffset) {
    auto expCtx = getExpCtx();

    auto spec = BSON("$dateFromString" << BSON("dateString"
                                               << "2017-07-28T10:47:52.912"
                                               << "timezone"
                                               << "-01:00"));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    auto dateVal = Date_t::fromMillisSinceEpoch(1501242472912);
    ASSERT_VALUE_EQ(Value(dateVal), dateExp->evaluate(Document{}));

    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "2017-07-28T10:47:52.912"
                                          << "timezone"
                                          << "+01:00"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    dateVal = Date_t::fromMillisSinceEpoch(1501235272912);
    ASSERT_VALUE_EQ(Value(dateVal), dateExp->evaluate(Document{}));

    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "2017-07-28T10:47:52.912"
                                          << "timezone"
                                          << "+0445"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    dateVal = Date_t::fromMillisSinceEpoch(1501221772912);
    ASSERT_VALUE_EQ(Value(dateVal), dateExp->evaluate(Document{}));

    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "2017-07-28T10:47:52.912"
                                          << "timezone"
                                          << "+10:45"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    dateVal = Date_t::fromMillisSinceEpoch(1501200172912);
    ASSERT_VALUE_EQ(Value(dateVal), dateExp->evaluate(Document{}));

    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "1945-07-28T10:47:52.912"
                                          << "timezone"
                                          << "-08:00"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    dateVal = Date_t::fromMillisSinceEpoch(-770879527088);
    ASSERT_VALUE_EQ(Value(dateVal), dateExp->evaluate(Document{}));
}