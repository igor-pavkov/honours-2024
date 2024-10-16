TEST(ObjectParsing, ShouldAcceptExpressionAsValue) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    auto object = ExpressionObject::parse(expCtx, BSON("a" << BSON("$and" << BSONArray())), vps);
    ASSERT_VALUE_EQ(object->serialize(false),
                    Value(Document{{"a", Document{{"$and", BSONArray()}}}}));
}