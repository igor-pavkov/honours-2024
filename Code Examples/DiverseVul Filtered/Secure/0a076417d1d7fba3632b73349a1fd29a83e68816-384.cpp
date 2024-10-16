Value ExpressionSetEquals::evaluate(const Document& root) const {
    const size_t n = vpOperand.size();
    const auto& valueComparator = getExpressionContext()->getValueComparator();
    ValueSet lhs = valueComparator.makeOrderedValueSet();

    for (size_t i = 0; i < n; i++) {
        const Value nextEntry = vpOperand[i]->evaluate(root);
        uassert(17044,
                str::stream() << "All operands of $setEquals must be arrays. One "
                              << "argument is of type: "
                              << typeName(nextEntry.getType()),
                nextEntry.isArray());

        if (i == 0) {
            lhs.insert(nextEntry.getArray().begin(), nextEntry.getArray().end());
        } else {
            ValueSet rhs = valueComparator.makeOrderedValueSet();
            rhs.insert(nextEntry.getArray().begin(), nextEntry.getArray().end());
            if (lhs.size() != rhs.size()) {
                return Value(false);
            }

            if (!std::equal(lhs.begin(), lhs.end(), rhs.begin(), valueComparator.getEqualTo())) {
                return Value(false);
            }
        }
    }
    return Value(true);
}