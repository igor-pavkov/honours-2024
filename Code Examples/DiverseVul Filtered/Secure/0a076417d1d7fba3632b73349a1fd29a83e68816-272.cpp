    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        BSONObj specObject = BSON("" << spec());
        BSONElement specElement = specObject.firstElement();
        VariablesParseState vps = expCtx->variablesParseState;
        ASSERT_THROWS(Expression::parseOperand(expCtx, specElement, vps), AssertionException);
    }