    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<ExpressionNary> expression = new ExpressionAdd(expCtx);
        populateOperands(expression);
        ASSERT_BSONOBJ_EQ(expectedResult(), toBson(expression->evaluate(Document())));
    }