TEST(ExpressionObjectEvaluate, ShouldRemoveFieldsThatHaveMissingValues) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto object = ExpressionObject::create(expCtx,
                                           {{"a", ExpressionFieldPath::create(expCtx, "a.b")},
                                            {"b", ExpressionFieldPath::create(expCtx, "missing")}});
    ASSERT_VALUE_EQ(Value(Document{}), object->evaluate(Document()));
    ASSERT_VALUE_EQ(Value(Document{}), object->evaluate(Document{{"a", 1}}));
}