intrusive_ptr<ExpressionCoerceToBool> ExpressionCoerceToBool::create(
    const intrusive_ptr<ExpressionContext>& expCtx, const intrusive_ptr<Expression>& pExpression) {
    intrusive_ptr<ExpressionCoerceToBool> pNew(new ExpressionCoerceToBool(expCtx, pExpression));
    return pNew;
}