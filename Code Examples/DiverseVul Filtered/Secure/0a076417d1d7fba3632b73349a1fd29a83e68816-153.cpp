static Value evaluateExpression(const string& expressionName,
                                const vector<ImplicitValue>& operands) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    const BSONObj obj = BSON(expressionName << ImplicitValue::convertToValue(operands));
    auto expression = Expression::parseExpression(expCtx, obj, vps);
    Value result = expression->evaluate(Document());
    return result;
}