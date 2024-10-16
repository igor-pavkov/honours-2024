intrusive_ptr<Expression> ExpressionMeta::parse(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONElement expr,
    const VariablesParseState& vpsIn) {
    uassert(17307, "$meta only supports string arguments", expr.type() == String);
    if (expr.valueStringData() == "textScore") {
        return new ExpressionMeta(expCtx, MetaType::TEXT_SCORE);
    } else if (expr.valueStringData() == "randVal") {
        return new ExpressionMeta(expCtx, MetaType::RAND_VAL);
    } else {
        uasserted(17308, "Unsupported argument to $meta: " + expr.String());
    }
}