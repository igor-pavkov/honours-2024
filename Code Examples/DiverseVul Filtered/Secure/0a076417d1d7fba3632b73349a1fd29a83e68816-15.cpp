intrusive_ptr<Expression> ExpressionNary::optimize() {
    uint32_t constOperandCount = 0;

    for (auto& operand : vpOperand) {
        operand = operand->optimize();
        if (dynamic_cast<ExpressionConstant*>(operand.get())) {
            ++constOperandCount;
        }
    }
    // If all the operands are constant expressions, collapse the expression into one constant
    // expression.
    if (constOperandCount == vpOperand.size()) {
        return intrusive_ptr<Expression>(
            ExpressionConstant::create(getExpressionContext(), evaluate(Document())));
    }

    // If the expression is associative, we can collapse all the consecutive constant operands into
    // one by applying the expression to those consecutive constant operands.
    // If the expression is also commutative we can reorganize all the operands so that all of the
    // constant ones are together (arbitrarily at the back) and we can collapse all of them into
    // one.
    if (isAssociative()) {
        ExpressionVector constExpressions;
        ExpressionVector optimizedOperands;
        for (size_t i = 0; i < vpOperand.size();) {
            intrusive_ptr<Expression> operand = vpOperand[i];
            // If the operand is a constant one, add it to the current list of consecutive constant
            // operands.
            if (dynamic_cast<ExpressionConstant*>(operand.get())) {
                constExpressions.push_back(operand);
                ++i;
                continue;
            }

            // If the operand is exactly the same type as the one we are currently optimizing and
            // is also associative, replace the expression for the operands it has.
            // E.g: sum(a, b, sum(c, d), e) => sum(a, b, c, d, e)
            ExpressionNary* nary = dynamic_cast<ExpressionNary*>(operand.get());
            if (nary && str::equals(nary->getOpName(), getOpName()) && nary->isAssociative()) {
                invariant(!nary->vpOperand.empty());
                vpOperand[i] = std::move(nary->vpOperand[0]);
                vpOperand.insert(
                    vpOperand.begin() + i + 1, nary->vpOperand.begin() + 1, nary->vpOperand.end());
                continue;
            }

            // If the operand is not a constant nor a same-type expression and the expression is
            // not commutative, evaluate an expression of the same type as the one we are
            // optimizing on the list of consecutive constant operands and use the resulting value
            // as a constant expression operand.
            // If the list of consecutive constant operands has less than 2 operands just place
            // back the operands.
            if (!isCommutative()) {
                if (constExpressions.size() > 1) {
                    ExpressionVector vpOperandSave = std::move(vpOperand);
                    vpOperand = std::move(constExpressions);
                    optimizedOperands.emplace_back(
                        ExpressionConstant::create(getExpressionContext(), evaluate(Document())));
                    vpOperand = std::move(vpOperandSave);
                } else {
                    optimizedOperands.insert(
                        optimizedOperands.end(), constExpressions.begin(), constExpressions.end());
                }
                constExpressions.clear();
            }
            optimizedOperands.push_back(operand);
            ++i;
        }

        if (constExpressions.size() > 1) {
            vpOperand = std::move(constExpressions);
            optimizedOperands.emplace_back(
                ExpressionConstant::create(getExpressionContext(), evaluate(Document())));
        } else {
            optimizedOperands.insert(
                optimizedOperands.end(), constExpressions.begin(), constExpressions.end());
        }

        vpOperand = std::move(optimizedOperands);
    }
    return this;
}