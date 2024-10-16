intrusive_ptr<Expression> ExpressionCond::parse(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONElement expr,
    const VariablesParseState& vps) {
    if (expr.type() != Object) {
        return Base::parse(expCtx, expr, vps);
    }
    verify(str::equals(expr.fieldName(), "$cond"));

    intrusive_ptr<ExpressionCond> ret = new ExpressionCond(expCtx);
    ret->vpOperand.resize(3);

    const BSONObj args = expr.embeddedObject();
    BSONForEach(arg, args) {
        if (str::equals(arg.fieldName(), "if")) {
            ret->vpOperand[0] = parseOperand(expCtx, arg, vps);
        } else if (str::equals(arg.fieldName(), "then")) {
            ret->vpOperand[1] = parseOperand(expCtx, arg, vps);
        } else if (str::equals(arg.fieldName(), "else")) {
            ret->vpOperand[2] = parseOperand(expCtx, arg, vps);
        } else {
            uasserted(17083,
                      str::stream() << "Unrecognized parameter to $cond: " << arg.fieldName());
        }
    }

    uassert(17080, "Missing 'if' parameter to $cond", ret->vpOperand[0]);
    uassert(17081, "Missing 'then' parameter to $cond", ret->vpOperand[1]);
    uassert(17082, "Missing 'else' parameter to $cond", ret->vpOperand[2]);

    return ret;
}