TEST_F(ExpressionDateToPartsTest, SerializesToObjectSyntax) {
    auto expCtx = getExpCtx();

    // Test that it serializes to the full format if given an object specification.
    BSONObj spec = BSON("$dateToParts" << BSON("date" << Date_t{} << "timezone"
                                                      << "Europe/London"
                                                      << "iso8601"
                                                      << false));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    auto expectedSerialization =
        Value(Document{{"$dateToParts",
                        Document{{"date", Document{{"$const", Date_t{}}}},
                                 {"timezone", Document{{"$const", "Europe/London"_sd}}},
                                 {"iso8601", Document{{"$const", false}}}}}});
    ASSERT_VALUE_EQ(dateExp->serialize(true), expectedSerialization);
    ASSERT_VALUE_EQ(dateExp->serialize(false), expectedSerialization);
}