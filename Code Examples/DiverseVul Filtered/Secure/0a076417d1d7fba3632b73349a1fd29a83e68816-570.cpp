    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression = ExpressionFieldPath::create(expCtx, "a.b.c");
        BSONArrayBuilder bab;
        bab << expression->serialize(false);
        assertBinaryEqual(BSON_ARRAY("$a.b.c"), bab.arr());
    }