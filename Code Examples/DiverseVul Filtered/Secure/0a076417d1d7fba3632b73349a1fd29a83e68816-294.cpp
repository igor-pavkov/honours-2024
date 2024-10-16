Value ExpressionSlice::evaluate(const Document& root) const {
    const size_t n = vpOperand.size();

    Value arrayVal = vpOperand[0]->evaluate(root);
    // Could be either a start index or the length from 0.
    Value arg2 = vpOperand[1]->evaluate(root);

    if (arrayVal.nullish() || arg2.nullish()) {
        return Value(BSONNULL);
    }

    uassert(28724,
            str::stream() << "First argument to $slice must be an array, but is"
                          << " of type: "
                          << typeName(arrayVal.getType()),
            arrayVal.isArray());
    uassert(28725,
            str::stream() << "Second argument to $slice must be a numeric value,"
                          << " but is of type: "
                          << typeName(arg2.getType()),
            arg2.numeric());
    uassert(28726,
            str::stream() << "Second argument to $slice can't be represented as"
                          << " a 32-bit integer: "
                          << arg2.coerceToDouble(),
            arg2.integral());

    const auto& array = arrayVal.getArray();
    size_t start;
    size_t end;

    if (n == 2) {
        // Only count given.
        int count = arg2.coerceToInt();
        start = 0;
        end = array.size();
        if (count >= 0) {
            end = std::min(end, size_t(count));
        } else {
            // Negative count's start from the back. If a abs(count) is greater
            // than the
            // length of the array, return the whole array.
            start = std::max(0, static_cast<int>(array.size()) + count);
        }
    } else {
        // We have both a start index and a count.
        int startInt = arg2.coerceToInt();
        if (startInt < 0) {
            // Negative values start from the back. If a abs(start) is greater
            // than the length
            // of the array, start from 0.
            start = std::max(0, static_cast<int>(array.size()) + startInt);
        } else {
            start = std::min(array.size(), size_t(startInt));
        }

        Value countVal = vpOperand[2]->evaluate(root);

        if (countVal.nullish()) {
            return Value(BSONNULL);
        }

        uassert(28727,
                str::stream() << "Third argument to $slice must be numeric, but "
                              << "is of type: "
                              << typeName(countVal.getType()),
                countVal.numeric());
        uassert(28728,
                str::stream() << "Third argument to $slice can't be represented"
                              << " as a 32-bit integer: "
                              << countVal.coerceToDouble(),
                countVal.integral());
        uassert(28729,
                str::stream() << "Third argument to $slice must be positive: "
                              << countVal.coerceToInt(),
                countVal.coerceToInt() > 0);

        size_t count = size_t(countVal.coerceToInt());
        end = std::min(start + count, array.size());
    }

    return Value(vector<Value>(array.begin() + start, array.begin() + end));
}