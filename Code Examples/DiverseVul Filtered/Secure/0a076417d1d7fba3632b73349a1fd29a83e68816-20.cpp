    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression = ExpressionConstant::create(expCtx, Value(5));
        DepsTracker dependencies;
        expression->addDependencies(&dependencies);
        ASSERT_EQUALS(0U, dependencies.fields.size());
        ASSERT_EQUALS(false, dependencies.needWholeDocument);
        ASSERT_EQUALS(false, dependencies.getNeedTextScore());
    }