    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression = ExpressionFieldPath::create(expCtx, "a");
        assertBinaryEqual(fromjson("{'':123}"),
                          toBson(expression->evaluate(fromBson(BSON("a" << 123)))));
    }