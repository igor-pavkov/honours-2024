boost::intrusive_ptr<Expression> parseObject(BSONObj specification) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;

    return Expression::parseObject(expCtx, specification, vps);
};