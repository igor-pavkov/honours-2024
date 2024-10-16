    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression = ExpressionConstant::create(expCtx, Value(5));
        assertBinaryEqual(BSON("" << 5), toBson(expression->evaluate(Document())));
    }