    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        BSONObj specObj = BSON("" << spec());
        BSONElement specElement = specObj.firstElement();
        VariablesParseState vps = expCtx->variablesParseState;
        intrusive_ptr<Expression> expression = Expression::parseOperand(expCtx, specElement, vps);
        ASSERT_BSONOBJ_EQ(constify(spec()), expressionToBson(expression));
        ASSERT_BSONOBJ_EQ(BSON("" << expectedResult()), toBson(expression->evaluate(Document())));
    }