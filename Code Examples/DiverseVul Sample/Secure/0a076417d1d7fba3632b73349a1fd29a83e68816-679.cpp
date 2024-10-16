Value ExpressionSetIsSubset::evaluate(const Document& root) const {
    const Value lhs = vpOperand[0]->evaluate(root);
    const Value rhs = vpOperand[1]->evaluate(root);

    uassert(17046,
            str::stream() << "both operands of $setIsSubset must be arrays. First "
                          << "argument is of type: "
                          << typeName(lhs.getType()),
            lhs.isArray());
    uassert(17042,
            str::stream() << "both operands of $setIsSubset must be arrays. Second "
                          << "argument is of type: "
                          << typeName(rhs.getType()),
            rhs.isArray());

    return setIsSubsetHelper(lhs.getArray(),
                             arrayToSet(rhs, getExpressionContext()->getValueComparator()));
}