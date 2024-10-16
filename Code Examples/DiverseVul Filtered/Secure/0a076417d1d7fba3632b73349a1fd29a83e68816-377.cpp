    void populateOperands(intrusive_ptr<ExpressionNary>& expression) {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        expression->addOperand(ExpressionConstant::create(expCtx, valueFromBson(operand())));
    }