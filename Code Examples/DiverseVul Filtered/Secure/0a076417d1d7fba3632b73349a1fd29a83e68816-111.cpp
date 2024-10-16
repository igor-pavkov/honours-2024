TEST_F(DateExpressionTest, ParsingRejectsUnrecognizedFieldsInObjectSpecification) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        BSONObj spec = BSON(expName << BSON("date" << Date_t{} << "timezone"
                                                   << "Europe/London"
                                                   << "extra"
                                                   << 4));
        ASSERT_THROWS_CODE(Expression::parseExpression(expCtx, spec, expCtx->variablesParseState),
                           AssertionException,
                           40535);
    }
}