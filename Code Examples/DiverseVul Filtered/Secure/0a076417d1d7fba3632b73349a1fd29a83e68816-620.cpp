    void addOperandArrayToExpr(const intrusive_ptr<Testable>& expr, const BSONArray& operands) {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        VariablesParseState vps = expCtx->variablesParseState;
        BSONObjIterator i(operands);
        while (i.more()) {
            BSONElement element = i.next();
            expr->addOperand(Expression::parseOperand(expCtx, element, vps));
        }
    }