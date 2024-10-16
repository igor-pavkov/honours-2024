    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression = ExpressionFieldPath::create(expCtx, "a.b");
        assertBinaryEqual(BSON("" << 55),
                          toBson(expression->evaluate(fromBson(BSON("a" << BSON("b" << 55))))));
    }