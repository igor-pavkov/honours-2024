Value ExpressionIndexOfArray::evaluate(const Document& root) const {
    Value arrayArg = vpOperand[0]->evaluate(root);

    if (arrayArg.nullish()) {
        return Value(BSONNULL);
    }

    uassert(40090,
            str::stream() << "$indexOfArray requires an array as a first argument, found: "
                          << typeName(arrayArg.getType()),
            arrayArg.isArray());

    std::vector<Value> array = arrayArg.getArray();

    Value searchItem = vpOperand[1]->evaluate(root);

    size_t startIndex = 0;
    if (vpOperand.size() > 2) {
        Value startIndexArg = vpOperand[2]->evaluate(root);
        uassertIfNotIntegralAndNonNegative(startIndexArg, getOpName(), "starting index");
        startIndex = static_cast<size_t>(startIndexArg.coerceToInt());
    }

    size_t endIndex = array.size();
    if (vpOperand.size() > 3) {
        Value endIndexArg = vpOperand[3]->evaluate(root);
        uassertIfNotIntegralAndNonNegative(endIndexArg, getOpName(), "ending index");
        // Don't let 'endIndex' exceed the length of the array.
        endIndex = std::min(array.size(), static_cast<size_t>(endIndexArg.coerceToInt()));
    }

    for (size_t i = startIndex; i < endIndex; i++) {
        if (getExpressionContext()->getValueComparator().evaluate(array[i] == searchItem)) {
            return Value(static_cast<int>(i));
        }
    }

    return Value(-1);
}