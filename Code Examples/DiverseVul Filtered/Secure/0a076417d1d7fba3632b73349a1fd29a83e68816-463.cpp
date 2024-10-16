    void addOperand(intrusive_ptr<ExpressionNary> expr, Value arg) {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        expr->addOperand(ExpressionConstant::create(expCtx, arg));
    }