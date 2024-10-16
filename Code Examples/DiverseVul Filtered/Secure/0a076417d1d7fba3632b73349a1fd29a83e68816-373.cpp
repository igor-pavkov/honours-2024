TEST(BuiltinRemoveVariableTest, RemoveSerializesCorrectlyWithTrailingPath) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    auto expression = ExpressionFieldPath::parse(expCtx, "$$REMOVE.a.b", vps);
    ASSERT_BSONOBJ_EQ(BSON("foo"
                           << "$$REMOVE.a.b"),
                      BSON("foo" << expression->serialize(false)));
}