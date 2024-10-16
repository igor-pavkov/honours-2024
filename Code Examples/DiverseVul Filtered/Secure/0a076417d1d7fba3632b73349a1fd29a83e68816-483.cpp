boost::intrusive_ptr<ExpressionCompare> ExpressionCompare::create(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    CmpOp cmpOp,
    const boost::intrusive_ptr<Expression>& exprLeft,
    const boost::intrusive_ptr<Expression>& exprRight) {
    boost::intrusive_ptr<ExpressionCompare> expr = new ExpressionCompare(expCtx, cmpOp);
    expr->vpOperand = {exprLeft, exprRight};
    return expr;
}