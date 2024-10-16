Value ExpressionNot::evaluate(const Document& root) const {
    Value pOp(vpOperand[0]->evaluate(root));

    bool b = pOp.coerceToBool();
    return Value(!b);
}