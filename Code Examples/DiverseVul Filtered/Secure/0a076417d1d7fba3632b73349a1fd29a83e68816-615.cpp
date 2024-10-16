intrusive_ptr<Expression> ExpressionCompare::parse(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONElement bsonExpr,
    const VariablesParseState& vps,
    CmpOp op) {
    intrusive_ptr<ExpressionCompare> expr = new ExpressionCompare(expCtx, op);
    ExpressionVector args = parseArguments(expCtx, bsonExpr, vps);
    expr->validateArguments(args);
    expr->vpOperand = args;
    return expr;
}