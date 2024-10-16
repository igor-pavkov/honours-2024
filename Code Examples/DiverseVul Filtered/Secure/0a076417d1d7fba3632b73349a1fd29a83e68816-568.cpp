Value ExpressionSetDifference::evaluate(const Document& root) const {
    const Value lhs = vpOperand[0]->evaluate(root);
    const Value rhs = vpOperand[1]->evaluate(root);

    if (lhs.nullish() || rhs.nullish()) {
        return Value(BSONNULL);
    }

    uassert(17048,
            str::stream() << "both operands of $setDifference must be arrays. First "
                          << "argument is of type: "
                          << typeName(lhs.getType()),
            lhs.isArray());
    uassert(17049,
            str::stream() << "both operands of $setDifference must be arrays. Second "
                          << "argument is of type: "
                          << typeName(rhs.getType()),
            rhs.isArray());

    ValueSet rhsSet = arrayToSet(rhs, getExpressionContext()->getValueComparator());
    const vector<Value>& lhsArray = lhs.getArray();
    vector<Value> returnVec;

    for (vector<Value>::const_iterator it = lhsArray.begin(); it != lhsArray.end(); ++it) {
        // rhsSet serves the dual role of filtering out elements that were originally present
        // in RHS and of eleminating duplicates from LHS
        if (rhsSet.insert(*it).second) {
            returnVec.push_back(*it);
        }
    }
    return Value(std::move(returnVec));
}