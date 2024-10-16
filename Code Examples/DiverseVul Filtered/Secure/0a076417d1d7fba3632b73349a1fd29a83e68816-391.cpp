Value ExpressionSplit::evaluate(const Document& root) const {
    Value inputArg = vpOperand[0]->evaluate(root);
    Value separatorArg = vpOperand[1]->evaluate(root);

    if (inputArg.nullish() || separatorArg.nullish()) {
        return Value(BSONNULL);
    }

    uassert(40085,
            str::stream() << "$split requires an expression that evaluates to a string as a first "
                             "argument, found: "
                          << typeName(inputArg.getType()),
            inputArg.getType() == BSONType::String);
    uassert(40086,
            str::stream() << "$split requires an expression that evaluates to a string as a second "
                             "argument, found: "
                          << typeName(separatorArg.getType()),
            separatorArg.getType() == BSONType::String);

    std::string input = inputArg.getString();
    std::string separator = separatorArg.getString();

    uassert(40087, "$split requires a non-empty separator", !separator.empty());

    std::vector<Value> output;

    // Keep track of the index at which the current output string began.
    size_t splitStartIndex = 0;

    // Iterate through 'input' and check to see if 'separator' matches at any point.
    for (size_t i = 0; i < input.size();) {
        if (stringHasTokenAtIndex(i, input, separator)) {
            // We matched; add the current string to our output and jump ahead.
            StringData splitString(input.c_str() + splitStartIndex, i - splitStartIndex);
            output.push_back(Value(splitString));
            i += separator.size();
            splitStartIndex = i;
        } else {
            // We did not match, continue to the next character.
            ++i;
        }
    }

    StringData splitString(input.c_str() + splitStartIndex, input.size() - splitStartIndex);
    output.push_back(Value(splitString));

    return Value(output);
}