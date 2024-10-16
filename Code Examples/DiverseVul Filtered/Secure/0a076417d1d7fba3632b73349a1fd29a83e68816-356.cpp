Value ExpressionIsArray::evaluate(const Document& root) const {
    Value argument = vpOperand[0]->evaluate(root);
    return Value(argument.isArray());
}