intrusive_ptr<ExpressionConstant> ExpressionConstant::create(
    const intrusive_ptr<ExpressionContext>& expCtx, const Value& value) {
    intrusive_ptr<ExpressionConstant> pEC(new ExpressionConstant(expCtx, value));
    return pEC;
}