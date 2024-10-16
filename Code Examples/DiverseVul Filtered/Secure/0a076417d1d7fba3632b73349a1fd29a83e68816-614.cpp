TEST(ExpressionConstantTest, ConstantOfValueMissingSerializesToRemoveSystemVar) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    intrusive_ptr<Expression> expression = ExpressionConstant::create(expCtx, Value());
    assertBinaryEqual(BSON("field"
                           << "$$REMOVE"),
                      BSON("field" << expression->serialize(false)));
}