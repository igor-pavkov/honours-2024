TEST(ExpressionConstantTest, ConstantOfValueMissingRemovesField) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    intrusive_ptr<Expression> expression = ExpressionConstant::create(expCtx, Value());
    assertBinaryEqual(BSONObj(), toBson(expression->evaluate(Document{{"foo", Value("bar"_sd)}})));
}