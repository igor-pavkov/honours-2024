TEST_F(ExpressionDateFromStringTest, OptimizesToConstantIfAllInputsAreConstant) {
    auto expCtx = getExpCtx();
    // Test that it becomes a constant with just the dateString.
    auto spec = BSON("$dateFromString" << BSON("dateString"
                                               << "2017-07-04T13:09:57Z"));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    Date_t dateVal = Date_t::fromMillisSinceEpoch(1499173797000);
    ASSERT_VALUE_EQ(Value(dateVal), dateExp->evaluate(Document{}));

    // Test that it becomes a constant with the dateString and timezone being a constant.
    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "2017-07-04T13:09:57"
                                          << "timezone"
                                          << "Europe/London"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    dateVal = Date_t::fromMillisSinceEpoch(1499170197000);
    ASSERT_VALUE_EQ(Value(dateVal), dateExp->evaluate(Document{}));

    // Test that it does *not* become a constant if dateString is not a constant.
    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "$date"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it does *not* become a constant if timezone is not a constant.
    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "2017-07-04T13:09:57Z"
                                          << "timezone"
                                          << "$tz"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));
}