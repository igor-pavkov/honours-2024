TEST_F(DateExpressionTest, DoesResultInNullIfGivenNullishInput) {
    // Make sure they each successfully evaluate with a different TimeZone.
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        auto contextDoc = Document{{"_id", 0}};

        // Test that the expression results in null if the date is nullish and the timezone is not
        // specified.
        auto spec = BSON(expName << BSON("date"
                                         << "$missing"));
        auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_VALUE_EQ(Value(BSONNULL), dateExp->evaluate(contextDoc));

        spec = BSON(expName << BSON("date" << BSONNULL));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_VALUE_EQ(Value(BSONNULL), dateExp->evaluate(contextDoc));

        spec = BSON(expName << BSON("date" << BSONUndefined));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_VALUE_EQ(Value(BSONNULL), dateExp->evaluate(contextDoc));

        // Test that the expression results in null if the date is present but the timezone is
        // nullish.
        spec = BSON(expName << BSON("date" << Date_t{} << "timezone"
                                           << "$missing"));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_VALUE_EQ(Value(BSONNULL), dateExp->evaluate(contextDoc));

        spec = BSON(expName << BSON("date" << Date_t{} << "timezone" << BSONNULL));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_VALUE_EQ(Value(BSONNULL), dateExp->evaluate(contextDoc));

        spec = BSON(expName << BSON("date" << Date_t{} << "timezone" << BSONUndefined));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_VALUE_EQ(Value(BSONNULL), dateExp->evaluate(contextDoc));

        // Test that the expression results in null if the date and timezone both nullish.
        spec = BSON(expName << BSON("date"
                                    << "$missing"
                                    << "timezone"
                                    << BSONUndefined));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_VALUE_EQ(Value(BSONNULL), dateExp->evaluate(contextDoc));

        // Test that the expression results in null if the date is nullish and timezone is present.
        spec = BSON(expName << BSON("date"
                                    << "$missing"
                                    << "timezone"
                                    << "Europe/London"));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_VALUE_EQ(Value(BSONNULL), dateExp->evaluate(contextDoc));
    }
}