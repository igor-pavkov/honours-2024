Value ExpressionIfNull::evaluate(const Document& root) const {
    Value pLeft(vpOperand[0]->evaluate(root));
    if (!pLeft.nullish())
        return pLeft;

    Value pRight(vpOperand[1]->evaluate(root));
    return pRight;
}