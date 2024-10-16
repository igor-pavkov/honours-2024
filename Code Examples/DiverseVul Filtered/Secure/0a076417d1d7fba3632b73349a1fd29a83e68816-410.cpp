TEST_F(ExpressionDateFromStringTest, SerializesToObjectSyntax) {
    auto expCtx = getExpCtx();

    // Test that it serializes to the full format if given an object specification.
    BSONObj spec = BSON("$dateFromString" << BSON("dateString"
                                                  << "2017-07-04T13:06:44Z"));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    auto expectedSerialization = Value(
        Document{{"$dateFromString",
                  Document{{"dateString", Document{{"$const", "2017-07-04T13:06:44Z"_sd}}}}}});

    ASSERT_VALUE_EQ(dateExp->serialize(true), expectedSerialization);
    ASSERT_VALUE_EQ(dateExp->serialize(false), expectedSerialization);

    // Test that it serializes to the full format if given an object specification.
    spec = BSON("$dateFromString" << BSON("dateString"
                                          << "2017-07-04T13:06:44Z"
                                          << "timezone"
                                          << "Europe/London"));
    dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    expectedSerialization =
        Value(Document{{"$dateFromString",
                        Document{{"dateString", Document{{"$const", "2017-07-04T13:06:44Z"_sd}}},
                                 {"timezone", Document{{"$const", "Europe/London"_sd}}}}}});

    ASSERT_VALUE_EQ(dateExp->serialize(true), expectedSerialization);
    ASSERT_VALUE_EQ(dateExp->serialize(false), expectedSerialization);
}