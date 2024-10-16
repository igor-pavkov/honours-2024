intrusive_ptr<Expression> Expression::parseObject(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONObj obj,
    const VariablesParseState& vps) {
    if (obj.isEmpty()) {
        return ExpressionObject::create(expCtx, {});
    }

    if (obj.firstElementFieldName()[0] == '$') {
        // Assume this is an expression like {$add: [...]}.
        return parseExpression(expCtx, obj, vps);
    }

    return ExpressionObject::parse(expCtx, obj, vps);
}