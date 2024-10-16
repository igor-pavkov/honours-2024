    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression = ExpressionFieldPath::create(expCtx, "a");
        assertBinaryEqual(fromjson("{}"), toBson(expression->evaluate(Document())));
    }