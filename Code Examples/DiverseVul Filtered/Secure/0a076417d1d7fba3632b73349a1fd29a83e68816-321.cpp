Value ExpressionSubstrCP::evaluate(const Document& root) const {
    Value inputVal(vpOperand[0]->evaluate(root));
    Value lowerVal(vpOperand[1]->evaluate(root));
    Value lengthVal(vpOperand[2]->evaluate(root));

    std::string str = inputVal.coerceToString();
    uassert(34450,
            str::stream() << getOpName() << ": starting index must be a numeric type (is BSON type "
                          << typeName(lowerVal.getType())
                          << ")",
            lowerVal.numeric());
    uassert(34451,
            str::stream() << getOpName()
                          << ": starting index cannot be represented as a 32-bit integral value: "
                          << lowerVal.toString(),
            lowerVal.integral());
    uassert(34452,
            str::stream() << getOpName() << ": length must be a numeric type (is BSON type "
                          << typeName(lengthVal.getType())
                          << ")",
            lengthVal.numeric());
    uassert(34453,
            str::stream() << getOpName()
                          << ": length cannot be represented as a 32-bit integral value: "
                          << lengthVal.toString(),
            lengthVal.integral());

    int startIndexCodePoints = lowerVal.coerceToInt();
    int length = lengthVal.coerceToInt();

    uassert(34454,
            str::stream() << getOpName() << ": length must be a nonnegative integer.",
            length >= 0);

    uassert(34455,
            str::stream() << getOpName() << ": the starting index must be nonnegative integer.",
            startIndexCodePoints >= 0);

    size_t startIndexBytes = 0;

    for (int i = 0; i < startIndexCodePoints; i++) {
        if (startIndexBytes >= str.size()) {
            return Value(StringData());
        }
        uassert(34456,
                str::stream() << getOpName() << ": invalid UTF-8 string",
                !str::isUTF8ContinuationByte(str[startIndexBytes]));
        size_t codePointLength = getCodePointLength(str[startIndexBytes]);
        uassert(
            34457, str::stream() << getOpName() << ": invalid UTF-8 string", codePointLength <= 4);
        startIndexBytes += codePointLength;
    }

    size_t endIndexBytes = startIndexBytes;

    for (int i = 0; i < length && endIndexBytes < str.size(); i++) {
        uassert(34458,
                str::stream() << getOpName() << ": invalid UTF-8 string",
                !str::isUTF8ContinuationByte(str[endIndexBytes]));
        size_t codePointLength = getCodePointLength(str[endIndexBytes]);
        uassert(
            34459, str::stream() << getOpName() << ": invalid UTF-8 string", codePointLength <= 4);
        endIndexBytes += codePointLength;
    }

    return Value(std::string(str, startIndexBytes, endIndexBytes - startIndexBytes));
}