TEST_F(ExpressionDateToStringTest, OptimizesToConstantIfAllInputsAreConstant) {
    auto expCtx = getExpCtx();

    // Test that it becomes a constant if both format and date are constant, and timezone is
    // missing.
    auto spec = BSON("$dateToString" << BSON("format"
                                             << "%Y-%m-%d"
                                             << "date"
                                             << Date_t{}));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it becomes a constant if both format, date and timezone are provided, and are both
    // constants.
    spec = BSON("$dateToString" << BSON("format"
                                        << "%Y-%m-%d"
                                        << "date"
                                        << Date_t{}
                                        << "timezone"
                                        << "Europe/Amsterdam"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it becomes a constant if both format, date and timezone are provided, and are both
    // expressions which evaluate to constants.
    spec = BSON("$dateToString" << BSON("format"
                                        << "%Y-%m%d"
                                        << "date"
                                        << BSON("$add" << BSON_ARRAY(Date_t{} << 1000))
                                        << "timezone"
                                        << BSON("$concat" << BSON_ARRAY("Europe"
                                                                        << "/"
                                                                        << "London"))));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it does *not* become a constant if both format, date and timezone are provided, but
    // date is not a constant.
    spec = BSON("$dateToString" << BSON("format"
                                        << "%Y-%m-%d"
                                        << "date"
                                        << "$date"
                                        << "timezone"
                                        << "Europe/London"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

    // Test that it does *not* become a constant if both format, date and timezone are provided, but
    // timezone is not a constant.
    spec = BSON("$dateToString" << BSON("format"
                                        << "%Y-%m-%d"
                                        << "date"
                                        << Date_t{}
                                        << "timezone"
                                        << "$tz"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));
}