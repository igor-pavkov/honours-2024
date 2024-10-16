Value ExpressionCompare::evaluate(const Document& root) const {
    Value pLeft(vpOperand[0]->evaluate(root));
    Value pRight(vpOperand[1]->evaluate(root));

    int cmp = getExpressionContext()->getValueComparator().compare(pLeft, pRight);

    // Make cmp one of 1, 0, or -1.
    if (cmp == 0) {
        // leave as 0
    } else if (cmp < 0) {
        cmp = -1;
    } else if (cmp > 0) {
        cmp = 1;
    }

    if (cmpOp == CMP)
        return Value(cmp);

    bool returnValue = cmpLookup[cmpOp].truthValue[cmp + 1];
    return Value(returnValue);
}