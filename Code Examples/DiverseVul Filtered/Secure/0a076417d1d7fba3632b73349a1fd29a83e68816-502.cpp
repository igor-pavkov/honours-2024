Value ExpressionCond::evaluate(const Document& root) const {
    Value pCond(vpOperand[0]->evaluate(root));
    int idx = pCond.coerceToBool() ? 1 : 2;
    return vpOperand[idx]->evaluate(root);
}