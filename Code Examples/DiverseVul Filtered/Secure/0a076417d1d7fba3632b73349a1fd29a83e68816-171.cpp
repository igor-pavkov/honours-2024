TEST_F(DateExpressionTest, SerializesToObjectSyntax) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        // Test that it serializes to the full format if given an object specification.
        BSONObj spec = BSON(expName << BSON("date" << Date_t{} << "timezone"
                                                   << "Europe/London"));
        auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        auto expectedSerialization =
            Value(Document{{expName,
                            Document{{"date", Document{{"$const", Date_t{}}}},
                                     {"timezone", Document{{"$const", "Europe/London"_sd}}}}}});
        ASSERT_VALUE_EQ(dateExp->serialize(true), expectedSerialization);
        ASSERT_VALUE_EQ(dateExp->serialize(false), expectedSerialization);

        // Test that it serializes to the full format if given a date.
        spec = BSON(expName << Date_t{});
        expectedSerialization =
            Value(Document{{expName, Document{{"date", Document{{"$const", Date_t{}}}}}}});
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_VALUE_EQ(dateExp->serialize(true), expectedSerialization);
        ASSERT_VALUE_EQ(dateExp->serialize(false), expectedSerialization);

        // Test that it serializes to the full format if given a date within an array.
        spec = BSON(expName << BSON_ARRAY(Date_t{}));
        dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        ASSERT_VALUE_EQ(dateExp->serialize(true), expectedSerialization);
        ASSERT_VALUE_EQ(dateExp->serialize(false), expectedSerialization);
    }
}