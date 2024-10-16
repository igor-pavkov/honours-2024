intrusive_ptr<ExpressionObject> ExpressionObject::create(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    vector<pair<string, intrusive_ptr<Expression>>>&& expressions) {
    return new ExpressionObject(expCtx, std::move(expressions));
}