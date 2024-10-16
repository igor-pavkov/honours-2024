    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<ExpressionNary> expression = new ExpressionAdd(expCtx);
        expression->addOperand(ExpressionConstant::create(expCtx, Value(2)));
        ASSERT_BSONOBJ_EQ(BSON("" << 2), toBson(expression->evaluate(Document())));
    }