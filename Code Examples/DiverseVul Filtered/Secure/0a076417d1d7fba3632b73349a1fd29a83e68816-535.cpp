Value ExpressionCoerceToBool::evaluate(const Document& root) const {
    Value pResult(pExpression->evaluate(root));
    bool b = pResult.coerceToBool();
    if (b)
        return Value(true);
    return Value(false);
}