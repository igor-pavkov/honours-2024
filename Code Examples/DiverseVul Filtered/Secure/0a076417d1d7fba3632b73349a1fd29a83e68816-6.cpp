    void run() {
        OptimizeBase::run();
        BSONObj specObject = BSON("" << spec());
        BSONElement specElement = specObject.firstElement();
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        VariablesParseState vps = expCtx->variablesParseState;
        intrusive_ptr<Expression> expression = Expression::parseOperand(expCtx, specElement, vps);
        // Check expression spec round trip.
        ASSERT_BSONOBJ_EQ(constify(spec()), expressionToBson(expression));
        // Check evaluation result.
        ASSERT_BSONOBJ_EQ(expectedResult(), toBson(expression->evaluate(Document())));
        // Check that the result is the same after optimizing.
        intrusive_ptr<Expression> optimized = expression->optimize();
        ASSERT_BSONOBJ_EQ(expectedResult(), toBson(optimized->evaluate(Document())));
    }