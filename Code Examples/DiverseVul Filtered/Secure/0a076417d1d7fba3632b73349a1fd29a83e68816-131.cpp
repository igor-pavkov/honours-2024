    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> nested = ExpressionFieldPath::create(expCtx, "a.b");
        intrusive_ptr<Expression> expression = ExpressionCoerceToBool::create(expCtx, nested);
        DepsTracker dependencies;
        expression->addDependencies(&dependencies);
        ASSERT_EQUALS(1U, dependencies.fields.size());
        ASSERT_EQUALS(1U, dependencies.fields.count("a.b"));
        ASSERT_EQUALS(false, dependencies.needWholeDocument);
        ASSERT_EQUALS(false, dependencies.getNeedTextScore());
    }