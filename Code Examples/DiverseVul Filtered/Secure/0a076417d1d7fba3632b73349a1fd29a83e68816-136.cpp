Value ExpressionIndexOfBytes::evaluate(const Document& root) const {
    Value stringArg = vpOperand[0]->evaluate(root);

    if (stringArg.nullish()) {
        return Value(BSONNULL);
    }

    uassert(40091,
            str::stream() << "$indexOfBytes requires a string as the first argument, found: "
                          << typeName(stringArg.getType()),
            stringArg.getType() == String);
    const std::string& input = stringArg.getString();

    Value tokenArg = vpOperand[1]->evaluate(root);
    uassert(40092,
            str::stream() << "$indexOfBytes requires a string as the second argument, found: "
                          << typeName(tokenArg.getType()),
            tokenArg.getType() == String);
    const std::string& token = tokenArg.getString();

    size_t startIndex = 0;
    if (vpOperand.size() > 2) {
        Value startIndexArg = vpOperand[2]->evaluate(root);
        uassertIfNotIntegralAndNonNegative(startIndexArg, getOpName(), "starting index");
        startIndex = static_cast<size_t>(startIndexArg.coerceToInt());
    }

    size_t endIndex = input.size();
    if (vpOperand.size() > 3) {
        Value endIndexArg = vpOperand[3]->evaluate(root);
        uassertIfNotIntegralAndNonNegative(endIndexArg, getOpName(), "ending index");
        // Don't let 'endIndex' exceed the length of the string.
        endIndex = std::min(input.size(), static_cast<size_t>(endIndexArg.coerceToInt()));
    }

    if (startIndex > input.length() || endIndex < startIndex) {
        return Value(-1);
    }

    size_t position = input.substr(0, endIndex).find(token, startIndex);
    if (position == std::string::npos) {
        return Value(-1);
    }

    return Value(static_cast<int>(position));
}