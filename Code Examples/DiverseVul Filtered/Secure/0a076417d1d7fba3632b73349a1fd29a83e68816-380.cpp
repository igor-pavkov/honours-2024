intrusive_ptr<Expression> ExpressionReduce::parse(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONElement expr,
    const VariablesParseState& vps) {
    uassert(40075,
            str::stream() << "$reduce requires an object as an argument, found: "
                          << typeName(expr.type()),
            expr.type() == Object);

    intrusive_ptr<ExpressionReduce> reduce(new ExpressionReduce(expCtx));

    // vpsSub is used only to parse 'in', which must have access to $$this and $$value.
    VariablesParseState vpsSub(vps);
    reduce->_thisVar = vpsSub.defineVariable("this");
    reduce->_valueVar = vpsSub.defineVariable("value");

    for (auto&& elem : expr.Obj()) {
        auto field = elem.fieldNameStringData();

        if (field == "input") {
            reduce->_input = parseOperand(expCtx, elem, vps);
        } else if (field == "initialValue") {
            reduce->_initial = parseOperand(expCtx, elem, vps);
        } else if (field == "in") {
            reduce->_in = parseOperand(expCtx, elem, vpsSub);
        } else {
            uasserted(40076, str::stream() << "$reduce found an unknown argument: " << field);
        }
    }

    uassert(40077, "$reduce requires 'input' to be specified", reduce->_input);
    uassert(40078, "$reduce requires 'initialValue' to be specified", reduce->_initial);
    uassert(40079, "$reduce requires 'in' to be specified", reduce->_in);

    return reduce;
}