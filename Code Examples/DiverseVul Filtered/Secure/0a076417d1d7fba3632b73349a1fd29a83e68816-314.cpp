Value ExpressionSubstrBytes::evaluate(const Document& root) const {
    Value pString(vpOperand[0]->evaluate(root));
    Value pLower(vpOperand[1]->evaluate(root));
    Value pLength(vpOperand[2]->evaluate(root));

    string str = pString.coerceToString();
    uassert(16034,
            str::stream() << getOpName()
                          << ":  starting index must be a numeric type (is BSON type "
                          << typeName(pLower.getType())
                          << ")",
            (pLower.getType() == NumberInt || pLower.getType() == NumberLong ||
             pLower.getType() == NumberDouble));
    uassert(16035,
            str::stream() << getOpName() << ":  length must be a numeric type (is BSON type "
                          << typeName(pLength.getType())
                          << ")",
            (pLength.getType() == NumberInt || pLength.getType() == NumberLong ||
             pLength.getType() == NumberDouble));

    string::size_type lower = static_cast<string::size_type>(pLower.coerceToLong());
    string::size_type length = static_cast<string::size_type>(pLength.coerceToLong());

    uassert(28656,
            str::stream() << getOpName()
                          << ":  Invalid range, starting index is a UTF-8 continuation byte.",
            (lower >= str.length() || !str::isUTF8ContinuationByte(str[lower])));

    // Check the byte after the last character we'd return. If it is a continuation byte, that
    // means we're in the middle of a UTF-8 character.
    uassert(
        28657,
        str::stream() << getOpName()
                      << ":  Invalid range, ending index is in the middle of a UTF-8 character.",
        (lower + length >= str.length() || !str::isUTF8ContinuationByte(str[lower + length])));

    if (lower >= str.length()) {
        // If lower > str.length() then string::substr() will throw out_of_range, so return an
        // empty string if lower is not a valid string index.
        return Value(StringData());
    }
    return Value(str.substr(lower, length));
}