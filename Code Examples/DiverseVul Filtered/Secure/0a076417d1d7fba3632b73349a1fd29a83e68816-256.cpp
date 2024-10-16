intrusive_ptr<Expression> ExpressionCoerceToBool::optimize() {
    /* optimize the operand */
    pExpression = pExpression->optimize();

    /* if the operand already produces a boolean, then we don't need this */
    /* LATER - Expression to support a "typeof" query? */
    Expression* pE = pExpression.get();
    if (dynamic_cast<ExpressionAnd*>(pE) || dynamic_cast<ExpressionOr*>(pE) ||
        dynamic_cast<ExpressionNot*>(pE) || dynamic_cast<ExpressionCoerceToBool*>(pE))
        return pExpression;

    return intrusive_ptr<Expression>(this);
}