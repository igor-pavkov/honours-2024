    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        ASSERT_THROWS(ExpressionFieldPath::create(expCtx, ""), AssertionException);
    }