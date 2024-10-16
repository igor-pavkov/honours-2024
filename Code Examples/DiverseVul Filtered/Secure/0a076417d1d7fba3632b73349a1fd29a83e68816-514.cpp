    void run() {
        BSONObj spec = BSON("IGNORED_FIELD_NAME"
                            << "foo");
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        BSONElement specElement = spec.firstElement();
        VariablesParseState vps = expCtx->variablesParseState;
        intrusive_ptr<Expression> expression = ExpressionConstant::parse(expCtx, specElement, vps);
        assertBinaryEqual(BSON(""
                               << "foo"),
                          toBson(expression->evaluate(Document())));
    }