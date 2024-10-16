Value ExpressionSetUnion::evaluate(const Document& root) const {
    ValueSet unionedSet = getExpressionContext()->getValueComparator().makeOrderedValueSet();
    const size_t n = vpOperand.size();
    for (size_t i = 0; i < n; i++) {
        const Value newEntries = vpOperand[i]->evaluate(root);
        if (newEntries.nullish()) {
            return Value(BSONNULL);
        }
        uassert(17043,
                str::stream() << "All operands of $setUnion must be arrays. One argument"
                              << " is of type: "
                              << typeName(newEntries.getType()),
                newEntries.isArray());

        unionedSet.insert(newEntries.getArray().begin(), newEntries.getArray().end());
    }
    return Value(vector<Value>(unionedSet.begin(), unionedSet.end()));
}