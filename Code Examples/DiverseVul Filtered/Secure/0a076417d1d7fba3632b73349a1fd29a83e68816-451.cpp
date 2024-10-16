Value ExpressionType::evaluate(const Document& root) const {
    Value val(vpOperand[0]->evaluate(root));
    return Value(StringData(typeName(val.getType())));
}