intrusive_ptr<Expression> parseOperand(BSONObj specification) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    BSONElement specElement = specification.firstElement();
    VariablesParseState vps = expCtx->variablesParseState;
    return Expression::parseOperand(expCtx, specElement, vps);
}