TEST_F(DateExpressionTest, OptimizesToConstantIfAllInputsAreConstant) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        // Test that it becomes a constant if only date is provided, and it is constant.
        auto spec = BSON(expName << BSON("date" << Date_t{}));
        auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

        // Test that it becomes a constant if both date and timezone are provided, and are both
        // constants.
        spec = BSON(expName << BSON("date" << Date_t{} << "timezone"
                                           << "Europe/London"));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

        // Test that it becomes a constant if both date and timezone are provided, and are both
        // expressions which evaluate to constants.
        spec = BSON(expName << BSON("date" << BSON("$add" << BSON_ARRAY(Date_t{} << 1000))
                                           << "timezone"
                                           << BSON("$concat" << BSON_ARRAY("Europe"
                                                                           << "/"
                                                                           << "London"))));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

        // Test that it does *not* become a constant if both date and timezone are provided, but
        // date is not a constant.
        spec = BSON(expName << BSON("date"
                                    << "$date"
                                    << "timezone"
                                    << "Europe/London"));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));

        // Test that it does *not* become a constant if both date and timezone are provided, but
        // timezone is not a constant.
        spec = BSON(expName << BSON("date" << Date_t{} << "timezone"
                                           << "$tz"));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_FALSE(dynamic_cast<ExpressionConstant*>(dateExp->optimize().get()));
    }
}