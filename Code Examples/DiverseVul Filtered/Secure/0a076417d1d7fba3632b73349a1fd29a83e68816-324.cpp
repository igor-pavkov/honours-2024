TEST(BuiltinRemoveVariableTest, RemoveSerializesCorrectlyAfterOptimization) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    auto expression = ExpressionFieldPath::parse(expCtx, "$$REMOVE.a.b", vps);
    auto optimizedExpression = expression->optimize();
    ASSERT(dynamic_cast<ExpressionConstant*>(optimizedExpression.get()));
    ASSERT_BSONOBJ_EQ(BSON("foo"
                           << "$$REMOVE"),
                      BSON("foo" << optimizedExpression->serialize(false)));
}