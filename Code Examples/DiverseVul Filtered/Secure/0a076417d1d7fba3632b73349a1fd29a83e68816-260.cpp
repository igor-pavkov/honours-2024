Value ExpressionIn::evaluate(const Document& root) const {
    Value argument(vpOperand[0]->evaluate(root));
    Value arrayOfValues(vpOperand[1]->evaluate(root));

    uassert(40081,
            str::stream() << "$in requires an array as a second argument, found: "
                          << typeName(arrayOfValues.getType()),
            arrayOfValues.isArray());
    for (auto&& value : arrayOfValues.getArray()) {
        if (getExpressionContext()->getValueComparator().evaluate(argument == value)) {
            return Value(true);
        }
    }
    return Value(false);
}