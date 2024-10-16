intrusive_ptr<Expression> ExpressionLet::parse(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONElement expr,
    const VariablesParseState& vpsIn) {
    verify(str::equals(expr.fieldName(), "$let"));

    uassert(16874, "$let only supports an object as its argument", expr.type() == Object);
    const BSONObj args = expr.embeddedObject();

    // varsElem must be parsed before inElem regardless of BSON order.
    BSONElement varsElem;
    BSONElement inElem;
    BSONForEach(arg, args) {
        if (str::equals(arg.fieldName(), "vars")) {
            varsElem = arg;
        } else if (str::equals(arg.fieldName(), "in")) {
            inElem = arg;
        } else {
            uasserted(16875,
                      str::stream() << "Unrecognized parameter to $let: " << arg.fieldName());
        }
    }

    uassert(16876, "Missing 'vars' parameter to $let", !varsElem.eoo());
    uassert(16877, "Missing 'in' parameter to $let", !inElem.eoo());

    // parse "vars"
    VariablesParseState vpsSub(vpsIn);  // vpsSub gets our vars, vpsIn doesn't.
    VariableMap vars;
    BSONForEach(varElem, varsElem.embeddedObjectUserCheck()) {
        const string varName = varElem.fieldName();
        Variables::uassertValidNameForUserWrite(varName);
        Variables::Id id = vpsSub.defineVariable(varName);

        vars[id] = NameAndExpression(varName,
                                     parseOperand(expCtx, varElem, vpsIn));  // only has outer vars
    }

    // parse "in"
    intrusive_ptr<Expression> subExpression = parseOperand(expCtx, inElem, vpsSub);  // has our vars

    return new ExpressionLet(expCtx, vars, subExpression);
}