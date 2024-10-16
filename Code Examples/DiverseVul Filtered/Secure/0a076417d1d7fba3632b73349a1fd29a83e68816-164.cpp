Value ExpressionArray::serialize(bool explain) const {
    vector<Value> expressions;
    expressions.reserve(vpOperand.size());
    for (auto&& expr : vpOperand) {
        expressions.push_back(expr->serialize(explain));
    }
    return Value(std::move(expressions));
}