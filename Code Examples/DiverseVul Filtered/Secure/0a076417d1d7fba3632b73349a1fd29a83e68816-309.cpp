boost::intrusive_ptr<Expression> parseExpression(BSONObj specification) {
    const boost::intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    return Expression::parseExpression(expCtx, specification, vps);
}