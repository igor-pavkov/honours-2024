intrusive_ptr<Expression> ExpressionPow::create(
    const boost::intrusive_ptr<ExpressionContext>& expCtx, Value base, Value exp) {
    intrusive_ptr<ExpressionPow> expr(new ExpressionPow(expCtx));
    expr->vpOperand.push_back(
        ExpressionConstant::create(expr->getExpressionContext(), std::move(base)));
    expr->vpOperand.push_back(
        ExpressionConstant::create(expr->getExpressionContext(), std::move(exp)));
    return expr;
}