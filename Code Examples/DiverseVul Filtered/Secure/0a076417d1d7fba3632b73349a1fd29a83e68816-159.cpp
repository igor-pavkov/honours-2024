    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        intrusive_ptr<Expression> expression = ExpressionFieldPath::create(expCtx, "a.b.c");
        assertBinaryEqual(fromjson("{'':[[1,2],3,[4],[[5]],[6,7]]}"),
                          toBson(expression->evaluate(fromBson(fromjson("{a:[{b:[{c:1},{c:2}]},"
                                                                        "{b:{c:3}},"
                                                                        "{b:[{c:4}]},"
                                                                        "{b:[{c:[5]}]},"
                                                                        "{b:{c:[6,7]}}]}")))));
    }