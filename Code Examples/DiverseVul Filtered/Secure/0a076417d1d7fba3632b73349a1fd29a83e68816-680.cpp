TEST_F(DateExpressionTest, AcceptsObjectIds) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        BSONObj spec = BSON(expName << "$oid");
        auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        auto contextDoc = Document{{"oid", OID::gen()}};
        dateExp->evaluate(contextDoc);  // Should not throw.
    }
}