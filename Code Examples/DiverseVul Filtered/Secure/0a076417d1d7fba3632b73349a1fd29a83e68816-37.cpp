    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression =
            ExpressionCoerceToBool::create(expCtx, ExpressionFieldPath::create(expCtx, "foo"));

        // serialized as $and because CoerceToBool isn't an ExpressionNary
        assertBinaryEqual(BSON_ARRAY(fromjson("{$and:['$foo']}")), toBsonArray(expression));
    }