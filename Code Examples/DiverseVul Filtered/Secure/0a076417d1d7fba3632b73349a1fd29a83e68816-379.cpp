TEST(BuiltinRemoveVariableTest, RemoveSerializesCorrectly) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    auto expression = ExpressionFieldPath::parse(expCtx, "$$REMOVE", vps);
    ASSERT_BSONOBJ_EQ(BSON("foo"
                           << "$$REMOVE"),
                      BSON("foo" << expression->serialize(false)));
}