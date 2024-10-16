    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression = ExpressionConstant::create(expCtx, Value(5));
        // An attempt to optimize returns the Expression itself.
        ASSERT_EQUALS(expression, expression->optimize());
    }