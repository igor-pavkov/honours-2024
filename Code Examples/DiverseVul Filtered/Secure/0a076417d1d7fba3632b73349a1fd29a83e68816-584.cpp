boost::intrusive_ptr<Expression> ExpressionSwitch::parse(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONElement expr,
    const VariablesParseState& vps) {
    uassert(40060,
            str::stream() << "$switch requires an object as an argument, found: "
                          << typeName(expr.type()),
            expr.type() == Object);

    intrusive_ptr<ExpressionSwitch> expression(new ExpressionSwitch(expCtx));

    for (auto&& elem : expr.Obj()) {
        auto field = elem.fieldNameStringData();

        if (field == "branches") {
            // Parse each branch separately.
            uassert(40061,
                    str::stream() << "$switch expected an array for 'branches', found: "
                                  << typeName(elem.type()),
                    elem.type() == Array);

            for (auto&& branch : elem.Array()) {
                uassert(40062,
                        str::stream() << "$switch expected each branch to be an object, found: "
                                      << typeName(branch.type()),
                        branch.type() == Object);

                ExpressionPair branchExpression;

                for (auto&& branchElement : branch.Obj()) {
                    auto branchField = branchElement.fieldNameStringData();

                    if (branchField == "case") {
                        branchExpression.first = parseOperand(expCtx, branchElement, vps);
                    } else if (branchField == "then") {
                        branchExpression.second = parseOperand(expCtx, branchElement, vps);
                    } else {
                        uasserted(40063,
                                  str::stream() << "$switch found an unknown argument to a branch: "
                                                << branchField);
                    }
                }

                uassert(40064,
                        "$switch requires each branch have a 'case' expression",
                        branchExpression.first);
                uassert(40065,
                        "$switch requires each branch have a 'then' expression.",
                        branchExpression.second);

                expression->_branches.push_back(branchExpression);
            }
        } else if (field == "default") {
            // Optional, arbitrary expression.
            expression->_default = parseOperand(expCtx, elem, vps);
        } else {
            uasserted(40067, str::stream() << "$switch found an unknown argument: " << field);
        }
    }

    uassert(40068, "$switch requires at least one branch.", !expression->_branches.empty());

    return expression;
}