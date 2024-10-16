    void populateOperands(intrusive_ptr<ExpressionNary>& expression) {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        expression->addOperand(
            ExpressionConstant::create(expCtx, valueFromBson(_reverse ? operand2() : operand1())));
        expression->addOperand(
            ExpressionConstant::create(expCtx, valueFromBson(_reverse ? operand1() : operand2())));
    }