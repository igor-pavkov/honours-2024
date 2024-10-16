intrusive_ptr<Expression> ExpressionConstant::parse(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONElement exprElement,
    const VariablesParseState& vps) {
    return new ExpressionConstant(expCtx, Value(exprElement));
}