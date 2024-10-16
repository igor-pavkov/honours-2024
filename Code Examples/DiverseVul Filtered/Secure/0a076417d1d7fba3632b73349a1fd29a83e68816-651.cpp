TEST_F(DateExpressionTest, AcceptsTimestamps) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        BSONObj spec = BSON(expName << "$ts");
        auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        auto contextDoc = Document{{"ts", Timestamp{Date_t{}}}};
        dateExp->evaluate(contextDoc);  // Should not throw.
    }
}