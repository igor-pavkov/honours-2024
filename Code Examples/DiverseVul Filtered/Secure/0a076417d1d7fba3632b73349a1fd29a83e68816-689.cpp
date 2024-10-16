Value ExpressionZip::evaluate(const Document& root) const {
    // Evaluate input values.
    vector<vector<Value>> inputValues;
    inputValues.reserve(_inputs.size());

    size_t minArraySize = 0;
    size_t maxArraySize = 0;
    for (size_t i = 0; i < _inputs.size(); i++) {
        Value evalExpr = _inputs[i]->evaluate(root);
        if (evalExpr.nullish()) {
            return Value(BSONNULL);
        }

        uassert(34468,
                str::stream() << "$zip found a non-array expression in input: "
                              << evalExpr.toString(),
                evalExpr.isArray());

        inputValues.push_back(evalExpr.getArray());

        size_t arraySize = evalExpr.getArrayLength();

        if (i == 0) {
            minArraySize = arraySize;
            maxArraySize = arraySize;
        } else {
            auto arraySizes = std::minmax({minArraySize, arraySize, maxArraySize});
            minArraySize = arraySizes.first;
            maxArraySize = arraySizes.second;
        }
    }

    vector<Value> evaluatedDefaults(_inputs.size(), Value(BSONNULL));

    // If we need default values, evaluate each expression.
    if (minArraySize != maxArraySize) {
        for (size_t i = 0; i < _defaults.size(); i++) {
            evaluatedDefaults[i] = _defaults[i]->evaluate(root);
        }
    }

    size_t outputLength = _useLongestLength ? maxArraySize : minArraySize;

    // The final output array, e.g. [[1, 2, 3], [2, 3, 4]].
    vector<Value> output;

    // Used to construct each array in the output, e.g. [1, 2, 3].
    vector<Value> outputChild;

    output.reserve(outputLength);
    outputChild.reserve(_inputs.size());

    for (size_t row = 0; row < outputLength; row++) {
        outputChild.clear();
        for (size_t col = 0; col < _inputs.size(); col++) {
            if (inputValues[col].size() > row) {
                // Add the value from the appropriate input array.
                outputChild.push_back(inputValues[col][row]);
            } else {
                // Add the corresponding default value.
                outputChild.push_back(evaluatedDefaults[col]);
            }
        }
        output.push_back(Value(outputChild));
    }

    return Value(output);
}