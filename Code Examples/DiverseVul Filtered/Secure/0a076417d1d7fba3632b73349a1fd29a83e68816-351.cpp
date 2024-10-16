TEST(ExpressionObjectEvaluate, EmptyObjectShouldEvaluateToEmptyDocument) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto object = ExpressionObject::create(expCtx, {});
    ASSERT_VALUE_EQ(Value(Document()), object->evaluate(Document()));
    ASSERT_VALUE_EQ(Value(Document()), object->evaluate(Document{{"a", 1}}));
    ASSERT_VALUE_EQ(Value(Document()), object->evaluate(Document{{"_id", "ID"_sd}}));
}