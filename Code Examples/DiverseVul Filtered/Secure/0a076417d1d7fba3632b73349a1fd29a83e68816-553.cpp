    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression = ExpressionFieldPath::create(expCtx, "a.b");
        assertBinaryEqual(fromjson("{'':[9]}"),
                          toBson(expression->evaluate(fromBson(fromjson("{a:[{b:9}]}")))));
    }