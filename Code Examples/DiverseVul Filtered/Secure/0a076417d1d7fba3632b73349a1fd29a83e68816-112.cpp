intrusive_ptr<ExpressionFieldPath> ExpressionFieldPath::create(
    const boost::intrusive_ptr<ExpressionContext>& expCtx, const string& fieldPath) {
    return new ExpressionFieldPath(expCtx, "CURRENT." + fieldPath, Variables::kRootId);
}