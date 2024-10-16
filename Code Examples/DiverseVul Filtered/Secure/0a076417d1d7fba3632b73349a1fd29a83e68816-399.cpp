TEST_F(ExpressionDateFromStringTest, RejectsUnparsableString) {
    auto expCtx = getExpCtx();

    auto spec = BSON("$dateFromString" << BSON("dateString"
                                               << "60.Monday1770/06:59"));
    auto dateExp = Expression::parseExpression(expCtx, spec, expCtx->variablesParseState);
    ASSERT_THROWS_CODE(dateExp->evaluate({}), AssertionException, 40553);
}