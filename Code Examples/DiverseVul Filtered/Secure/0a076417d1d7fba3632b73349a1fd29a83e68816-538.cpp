    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> nested = ExpressionConstant::create(expCtx, Value(5));
        intrusive_ptr<Expression> expression = ExpressionCoerceToBool::create(expCtx, nested);
        ASSERT(expression->evaluate(Document()).getBool());
    }