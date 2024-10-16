    virtual void assertEvaluates(Value input, Value output) override {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        _expr = new ExpressionFloor(expCtx);
        ExpressionNaryTestOneArg::assertEvaluates(input, output);
    }