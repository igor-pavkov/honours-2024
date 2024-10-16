Value ExpressionToUpper::evaluate(const Document& root) const {
    Value pString(vpOperand[0]->evaluate(root));
    string str(pString.coerceToString());
    boost::to_upper(str);
    return Value(str);
}