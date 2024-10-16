Value ExpressionFilter::evaluate(const Document& root) const {
    // We are guaranteed at parse time that this isn't using our _varId.
    const Value inputVal = _input->evaluate(root);
    if (inputVal.nullish())
        return Value(BSONNULL);

    uassert(28651,
            str::stream() << "input to $filter must be an array not "
                          << typeName(inputVal.getType()),
            inputVal.isArray());

    const vector<Value>& input = inputVal.getArray();

    if (input.empty())
        return inputVal;

    vector<Value> output;
    auto& vars = getExpressionContext()->variables;
    for (const auto& elem : input) {
        vars.setValue(_varId, elem);

        if (_filter->evaluate(root).coerceToBool()) {
            output.push_back(std::move(elem));
        }
    }

    return Value(std::move(output));
}