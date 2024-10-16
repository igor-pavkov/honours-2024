Value ExpressionIndexOfCP::evaluate(const Document& root) const {
    Value stringArg = vpOperand[0]->evaluate(root);

    if (stringArg.nullish()) {
        return Value(BSONNULL);
    }

    uassert(40093,
            str::stream() << "$indexOfCP requires a string as the first argument, found: "
                          << typeName(stringArg.getType()),
            stringArg.getType() == String);
    const std::string& input = stringArg.getString();

    Value tokenArg = vpOperand[1]->evaluate(root);
    uassert(40094,
            str::stream() << "$indexOfCP requires a string as the second argument, found: "
                          << typeName(tokenArg.getType()),
            tokenArg.getType() == String);
    const std::string& token = tokenArg.getString();

    size_t startCodePointIndex = 0;
    if (vpOperand.size() > 2) {
        Value startIndexArg = vpOperand[2]->evaluate(root);
        uassertIfNotIntegralAndNonNegative(startIndexArg, getOpName(), "starting index");
        startCodePointIndex = static_cast<size_t>(startIndexArg.coerceToInt());
    }

    // Compute the length (in code points) of the input, and convert 'startCodePointIndex' to a byte
    // index.
    size_t codePointLength = 0;
    size_t startByteIndex = 0;
    for (size_t byteIx = 0; byteIx < input.size(); ++codePointLength) {
        if (codePointLength == startCodePointIndex) {
            // We have determined the byte at which our search will start.
            startByteIndex = byteIx;
        }

        uassert(40095,
                "$indexOfCP found bad UTF-8 in the input",
                !str::isUTF8ContinuationByte(input[byteIx]));
        byteIx += getCodePointLength(input[byteIx]);
    }

    size_t endCodePointIndex = codePointLength;
    if (vpOperand.size() > 3) {
        Value endIndexArg = vpOperand[3]->evaluate(root);
        uassertIfNotIntegralAndNonNegative(endIndexArg, getOpName(), "ending index");

        // Don't let 'endCodePointIndex' exceed the number of code points in the string.
        endCodePointIndex =
            std::min(codePointLength, static_cast<size_t>(endIndexArg.coerceToInt()));
    }

    if (startByteIndex == 0 && input.empty() && token.empty()) {
        // If we are finding the index of "" in the string "", the below loop will not loop, so we
        // need a special case for this.
        return Value(0);
    }

    // We must keep track of which byte, and which code point, we are examining, being careful not
    // to overflow either the length of the string or the ending code point.

    size_t currentCodePointIndex = startCodePointIndex;
    for (size_t byteIx = startByteIndex; currentCodePointIndex < endCodePointIndex;
         ++currentCodePointIndex) {
        if (stringHasTokenAtIndex(byteIx, input, token)) {
            return Value(static_cast<int>(currentCodePointIndex));
        }
        byteIx += getCodePointLength(input[byteIx]);
    }

    return Value(-1);
}