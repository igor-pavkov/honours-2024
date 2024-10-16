    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<ExpressionNary> expression = new ExpressionAdd(expCtx);
        expression->addOperand(ExpressionConstant::create(expCtx, Value(true)));
        ASSERT_THROWS(expression->evaluate(Document()), AssertionException);
    }