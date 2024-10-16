    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression = ExpressionFieldPath::create(expCtx, "a.b.c");
        assertBinaryEqual(BSON("foo"
                               << "$a.b.c"),
                          BSON("foo" << expression->serialize(false)));
    }