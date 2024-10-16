Value ExpressionToLower::evaluate(const Document& root) const {
    Value pString(vpOperand[0]->evaluate(root));
    string str = pString.coerceToString();
    boost::to_lower(str);
    return Value(str);
}