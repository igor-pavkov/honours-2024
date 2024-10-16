    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression = ExpressionConstant::create(expCtx, Value(5));
        // The constant is copied out as is.
        assertBinaryEqual(constify(BSON_ARRAY(5)), toBsonArray(expression));
    }