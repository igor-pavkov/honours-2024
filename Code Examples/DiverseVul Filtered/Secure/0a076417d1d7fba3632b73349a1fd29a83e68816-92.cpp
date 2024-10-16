TEST(ExpressionObjectEvaluate, ShouldEvaluateEachField) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto object =
        ExpressionObject::create(expCtx, {{"a", makeConstant(1)}, {"b", makeConstant(5)}});
    ASSERT_VALUE_EQ(Value(Document{{"a", 1}, {"b", 5}}), object->evaluate(Document()));
    ASSERT_VALUE_EQ(Value(Document{{"a", 1}, {"b", 5}}), object->evaluate(Document{{"a", 1}}));
    ASSERT_VALUE_EQ(Value(Document{{"a", 1}, {"b", 5}}),
                    object->evaluate(Document{{"_id", "ID"_sd}}));
}