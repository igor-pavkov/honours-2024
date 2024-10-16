    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        BSONObj specObject = BSON("" << spec());
        BSONElement specElement = specObject.firstElement();
        VariablesParseState vps = expCtx->variablesParseState;
        intrusive_ptr<Expression> expression = Expression::parseOperand(expCtx, specElement, vps);
        ASSERT_BSONOBJ_EQ(constify(spec()), expressionToBson(expression));
        ASSERT_BSONOBJ_EQ(BSON("" << expectedResult()),
                          toBson(expression->evaluate(fromBson(BSON("a" << 1)))));
        intrusive_ptr<Expression> optimized = expression->optimize();
        ASSERT_BSONOBJ_EQ(BSON("" << expectedResult()),
                          toBson(optimized->evaluate(fromBson(BSON("a" << 1)))));
    }