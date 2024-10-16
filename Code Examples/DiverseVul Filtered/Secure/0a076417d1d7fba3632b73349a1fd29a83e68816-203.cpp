Value ExpressionMap::evaluate(const Document& root) const {
    // guaranteed at parse time that this isn't using our _varId
    const Value inputVal = _input->evaluate(root);
    if (inputVal.nullish())
        return Value(BSONNULL);

    uassert(16883,
            str::stream() << "input to $map must be an array not " << typeName(inputVal.getType()),
            inputVal.isArray());

    const vector<Value>& input = inputVal.getArray();

    if (input.empty())
        return inputVal;

    vector<Value> output;
    output.reserve(input.size());
    for (size_t i = 0; i < input.size(); i++) {
        getExpressionContext()->variables.setValue(_varId, input[i]);

        Value toInsert = _each->evaluate(root);
        if (toInsert.missing())
            toInsert = Value(BSONNULL);  // can't insert missing values into array

        output.push_back(toInsert);
    }

    return Value(std::move(output));
}