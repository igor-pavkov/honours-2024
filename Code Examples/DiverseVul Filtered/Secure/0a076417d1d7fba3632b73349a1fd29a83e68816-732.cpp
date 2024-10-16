Value ExpressionReduce::evaluate(const Document& root) const {
    Value inputVal = _input->evaluate(root);

    if (inputVal.nullish()) {
        return Value(BSONNULL);
    }

    uassert(40080,
            str::stream() << "$reduce requires that 'input' be an array, found: "
                          << inputVal.toString(),
            inputVal.isArray());

    Value accumulatedValue = _initial->evaluate(root);
    auto& vars = getExpressionContext()->variables;

    for (auto&& elem : inputVal.getArray()) {
        vars.setValue(_thisVar, elem);
        vars.setValue(_valueVar, accumulatedValue);

        accumulatedValue = _in->evaluate(root);
    }

    return accumulatedValue;
}