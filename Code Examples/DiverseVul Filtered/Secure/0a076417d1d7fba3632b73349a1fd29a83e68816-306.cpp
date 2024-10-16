intrusive_ptr<Expression> ExpressionZip::parse(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONElement expr,
    const VariablesParseState& vps) {
    uassert(34460,
            str::stream() << "$zip only supports an object as an argument, found "
                          << typeName(expr.type()),
            expr.type() == Object);

    intrusive_ptr<ExpressionZip> newZip(new ExpressionZip(expCtx));

    for (auto&& elem : expr.Obj()) {
        const auto field = elem.fieldNameStringData();
        if (field == "inputs") {
            uassert(34461,
                    str::stream() << "inputs must be an array of expressions, found "
                                  << typeName(elem.type()),
                    elem.type() == Array);
            for (auto&& subExpr : elem.Array()) {
                newZip->_inputs.push_back(parseOperand(expCtx, subExpr, vps));
            }
        } else if (field == "defaults") {
            uassert(34462,
                    str::stream() << "defaults must be an array of expressions, found "
                                  << typeName(elem.type()),
                    elem.type() == Array);
            for (auto&& subExpr : elem.Array()) {
                newZip->_defaults.push_back(parseOperand(expCtx, subExpr, vps));
            }
        } else if (field == "useLongestLength") {
            uassert(34463,
                    str::stream() << "useLongestLength must be a bool, found "
                                  << typeName(expr.type()),
                    elem.type() == Bool);
            newZip->_useLongestLength = elem.Bool();
        } else {
            uasserted(34464,
                      str::stream() << "$zip found an unknown argument: " << elem.fieldName());
        }
    }

    uassert(34465, "$zip requires at least one input array", !newZip->_inputs.empty());
    uassert(34466,
            "cannot specify defaults unless useLongestLength is true",
            (newZip->_useLongestLength || newZip->_defaults.empty()));
    uassert(34467,
            "defaults and inputs must have the same length",
            (newZip->_defaults.empty() || newZip->_defaults.size() == newZip->_inputs.size()));

    return std::move(newZip);
}