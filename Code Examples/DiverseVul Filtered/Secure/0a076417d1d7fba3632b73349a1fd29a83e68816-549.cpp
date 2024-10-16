TEST_F(DateExpressionTest, RejectsTypesThatCannotCoerceToDate) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        BSONObj spec = BSON(expName << "$stringField");
        auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
        auto contextDoc = Document{{"stringField", "string"_sd}};
        ASSERT_THROWS_CODE(dateExp->evaluate(contextDoc), AssertionException, 16006);
    }
}