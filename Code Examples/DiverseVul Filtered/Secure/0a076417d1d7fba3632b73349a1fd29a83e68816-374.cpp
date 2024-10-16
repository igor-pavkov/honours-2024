TEST_F(DateExpressionTest, ParsingRejectsEmptyObjectSpecification) {
    auto expCtx = getExpCtx();
    for (auto&& expName : dateExpressions) {
        BSONObj spec = BSON(expName << BSONObj());
        ASSERT_THROWS_CODE(Expression::parseExpression(expCtx, spec, expCtx->variablesParseState),
                           AssertionException,
                           40539);
    }
}