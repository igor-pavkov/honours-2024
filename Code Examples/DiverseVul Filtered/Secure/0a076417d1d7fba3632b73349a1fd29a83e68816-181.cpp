Value ExpressionAnd::evaluate(const Document& root) const {
    const size_t n = vpOperand.size();
    for (size_t i = 0; i < n; ++i) {
        Value pValue(vpOperand[i]->evaluate(root));
        if (!pValue.coerceToBool())
            return Value(false);
    }

    return Value(true);
}