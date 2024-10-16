TEST_F(DateExpressionTest, ParsingAcceptsAllFormats) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        auto possibleSyntaxes = {
            // Single argument.
            BSON(expName << Date_t{}),
            BSON(expName << "$date"),
            BSON(expName << BSON("$add" << BSON_ARRAY(Date_t{} << 1000))),
            // Single argument wrapped in an array.
            BSON(expName << BSON_ARRAY("$date")),
            BSON(expName << BSON_ARRAY(Date_t{})),
            BSON(expName << BSON_ARRAY(BSON("$add" << BSON_ARRAY(Date_t{} << 1000)))),
            // Object literal syntax.
            BSON(expName << BSON("date" << Date_t{})),
            BSON(expName << BSON("date"
                                 << "$date")),
            BSON(expName << BSON("date" << BSON("$add" << BSON_ARRAY("$date" << 1000)))),
            BSON(expName << BSON("date" << Date_t{} << "timezone"
                                        << "Europe/London")),
            BSON(expName << BSON("date" << Date_t{} << "timezone"
                                        << "$tz"))};
        for (auto&& syntax : possibleSyntaxes) {
            Expression::parseExpression(expCtx, syntax, expCtx->variablesParseState);
        }
    }
}