    virtual void assertEvaluates(Value input, Value output) override {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        _expr = new ExpressionTrunc(expCtx);
        ExpressionNaryTestOneArg::assertEvaluates(input, output);
    }