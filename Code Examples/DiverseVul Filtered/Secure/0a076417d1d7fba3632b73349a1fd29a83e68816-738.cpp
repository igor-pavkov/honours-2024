TEST_F(ExpressionDateFromPartsTest, SerializesToObjectSyntax) {
    auto expCtx = getExpCtx();

    // Test that it serializes to the full format if given an object specification.
    BSONObj spec =
        BSON("$dateFromParts" << BSON(
                 "year" << 2017 << "month" << 6 << "day" << 27 << "hour" << 14 << "minute" << 37
                        << "second"
                        << 15
                        << "millisecond"
                        << 414
                        << "timezone"
                        << "America/Los_Angeles"));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    auto expectedSerialization =
        Value(Document{{"$dateFromParts",
                        Document{{"year", Document{{"$const", 2017}}},
                                 {"month", Document{{"$const", 6}}},
                                 {"day", Document{{"$const", 27}}},
                                 {"hour", Document{{"$const", 14}}},
                                 {"minute", Document{{"$const", 37}}},
                                 {"second", Document{{"$const", 15}}},
                                 {"millisecond", Document{{"$const", 414}}},
                                 {"timezone", Document{{"$const", "America/Los_Angeles"_sd}}}}}});
    ASSERT_VALUE_EQ(dateExp->serialize(true), expectedSerialization);
    ASSERT_VALUE_EQ(dateExp->serialize(false), expectedSerialization);
}