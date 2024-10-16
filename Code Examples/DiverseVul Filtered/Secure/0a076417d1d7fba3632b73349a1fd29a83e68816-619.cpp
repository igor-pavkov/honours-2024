TEST_F(ExpressionDateToStringTest, SerializesToObjectSyntax) {
    auto expCtx = getExpCtx();

    // Test that it serializes to the full format if given an object specification.
    BSONObj spec = BSON("$dateToString" << BSON("date" << Date_t{} << "timezone"
                                                       << "Europe/London"
                                                       << "format"
                                                       << "%Y-%m-%d"));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    auto expectedSerialization =
        Value(Document{{"$dateToString",
                        Document{{"format", "%Y-%m-%d"_sd},
                                 {"date", Document{{"$const", Date_t{}}}},
                                 {"timezone", Document{{"$const", "Europe/London"_sd}}}}}});

    ASSERT_VALUE_EQ(dateExp->serialize(true), expectedSerialization);
    ASSERT_VALUE_EQ(dateExp->serialize(false), expectedSerialization);
}