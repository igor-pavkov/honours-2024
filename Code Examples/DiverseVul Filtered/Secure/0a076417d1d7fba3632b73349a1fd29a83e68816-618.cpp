TEST_F(ExpressionDateToPartsTest, OptimizesToConstantIfAllInputsAreConstant) {
    auto expCtx = getExpCtx();
    auto spec = BSON("$dateToParts" << BSON("date" << Date_t{}));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it becomes a constant if both date and timezone are provided, and are both
    // constants.
    spec = BSON("$dateToParts" << BSON("date" << Date_t{} << "timezone"
                                              << "UTC"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it becomes a constant if both date and timezone are provided, and are both
    // expressions which evaluate to constants.
    spec = BSON("$dateToParts" << BSON("date" << BSON("$add" << BSON_ARRAY(Date_t{} << 1000))
                                              << "timezone"
                                              << BSON("$concat" << BSON_ARRAY("Europe"
                                                                              << "/"
                                                                              << "London"))));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it becomes a constant if both date and iso8601 are provided, and are both
    // constants.
    spec = BSON("$dateToParts" << BSON("date" << Date_t{} << "iso8601" << true));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it becomes a constant if both date and iso8601 are provided, and are both
    // expressions which evaluate to constants.
    spec = BSON("$dateToParts" << BSON("date" << BSON("$add" << BSON_ARRAY(Date_t{} << 1000))
                                              << "iso8601"
                                              << BSON("$not" << false)));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it does *not* become a constant if both date and timezone are provided, but
    // date is not a constant.
    spec = BSON("$dateToParts" << BSON("date"
                                       << "$date"
                                       << "timezone"
                                       << "Europe/London"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it does *not* become a constant if both date and timezone are provided, but
    // timezone is not a constant.
    spec = BSON("$dateToParts" << BSON("date" << Date_t{} << "timezone"
                                              << "$tz"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it does *not* become a constant if both date and iso8601 are provided, but
    // iso8601 is not a constant.
    spec = BSON("$dateToParts" << BSON("date" << Date_t{} << "iso8601"
                                              << "$iso8601"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));
}