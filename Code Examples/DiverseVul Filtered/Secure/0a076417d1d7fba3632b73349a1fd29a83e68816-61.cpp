TEST_F(DateExpressionTest, DoesRespectTimeZone) {
    // Make sure they each successfully evaluate with a different TimeZone.
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        auto spec = BSON(expName << BSON("date" << Date_t{} << "timezone"
                                                << "America/New_York"));
        auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        auto contextDoc = Document{{"_id", 0}};
        dateExp->evaluate(contextDoc);  // Should not throw.
    }

    // Make sure the time zone is used during evaluation.
    auto date = Date_t::fromMillisSinceEpoch(1496777923000LL);  // 2017-06-06T19:38:43:234Z.
    auto specWithoutTimezone = BSON("$hour" << BSON("date" << date));
    auto hourWithoutTimezone =
        Expression::parseExpression(expCtx, specWithoutTimezone, expCtx->variablesParseState)
            ->evaluate({});
    ASSERT_VALUE_EQ(hourWithoutTimezone, Value(19));

    auto specWithTimezone = BSON("$hour" << BSON("date" << date << "timezone"
                                                        << "America/New_York"));
    auto hourWithTimezone =
        Expression::parseExpression(expCtx, specWithTimezone, expCtx->variablesParseState)
            ->evaluate({});
    ASSERT_VALUE_EQ(hourWithTimezone, Value(15));
}