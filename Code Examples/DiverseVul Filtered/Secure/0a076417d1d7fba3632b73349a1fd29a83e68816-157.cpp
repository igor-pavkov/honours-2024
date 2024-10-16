    void run() {
        BSONObj specObject = BSON("" << spec());
        BSONElement specElement = specObject.firstElement();
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        VariablesParseState vps = expCtx->variablesParseState;
        intrusive_ptr<Expression> expression = Expression::parseOperand(expCtx, specElement, vps);
        intrusive_ptr<Expression> optimized = expression->optimize();
        ASSERT_BSONOBJ_EQ(constify(expectedOptimized()), expressionToBson(optimized));
    }