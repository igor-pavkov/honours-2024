Value ExpressionArray::evaluate(const Document& root) const {
    vector<Value> values;
    values.reserve(vpOperand.size());
    for (auto&& expr : vpOperand) {
        Value elemVal = expr->evaluate(root);
        values.push_back(elemVal.missing() ? Value(BSONNULL) : std::move(elemVal));
    }
    return Value(std::move(values));
}