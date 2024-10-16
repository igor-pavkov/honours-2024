TEST_F(ExpressionDateFromPartsTest, OptimizesToConstantIfAllInputsAreConstant) {
    auto expCtx = getExpCtx();
    auto spec = BSON("$dateFromParts" << BSON("year" << 2017));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it becomes a constant if both year, month and day are provided, and are both
    // constants.
    spec = BSON("$dateFromParts" << BSON("year" << 2017 << "month" << 6 << "day" << 27));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it becomes a constant if both year, hour and minute are provided, and are both
    // expressions which evaluate to constants.
    spec = BSON("$dateFromParts" << BSON("year" << BSON("$add" << BSON_ARRAY(1900 << 107)) << "hour"
                                                << BSON("$add" << BSON_ARRAY(13 << 1))
                                                << "minute"
                                                << BSON("$add" << BSON_ARRAY(40 << 3))));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it becomes a constant if both year and milliseconds are provided, and year is an
    // expressions which evaluate to a constant, with milliseconds a constant
    spec = BSON("$dateFromParts" << BSON(
                    "year" << BSON("$add" << BSON_ARRAY(1900 << 107)) << "millisecond" << 514));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it becomes a constant if both isoWeekYear, and isoWeek are provided, and are both
    // constants.
    spec = BSON("$dateFromParts" << BSON("isoWeekYear" << 2017 << "isoWeek" << 26));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it becomes a constant if both isoWeekYear, isoWeek and isoDayOfWeek are provided,
    // and are both expressions which evaluate to constants.
    spec = BSON("$dateFromParts" << BSON("isoWeekYear" << BSON("$add" << BSON_ARRAY(1017 << 1000))
                                                       << "isoWeek"
                                                       << BSON("$add" << BSON_ARRAY(20 << 6))
                                                       << "isoDayOfWeek"
                                                       << BSON("$add" << BSON_ARRAY(3 << 2))));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it does *not* become a constant if both year and month are provided, but
    // year is not a constant.
    spec = BSON("$dateFromParts" << BSON("year"
                                         << "$year"
                                         << "month"
                                         << 6));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it does *not* become a constant if both year and day are provided, but
    // day is not a constant.
    spec = BSON("$dateFromParts" << BSON("year" << 2017 << "day"
                                                << "$day"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it does *not* become a constant if both isoWeekYear and isoDayOfWeek are provided,
    // but
    // isoDayOfWeek is not a constant.
    spec = BSON("$dateFromParts" << BSON("isoWeekYear" << 2017 << "isoDayOfWeek"
                                                       << "$isoDayOfWeekday"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));
}