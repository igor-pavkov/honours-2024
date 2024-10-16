TEST(ExpressionObjectEvaluate, ShouldEvaluateToEmptyDocumentIfAllFieldsAreMissing) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto object =
        ExpressionObject::create(expCtx, {{"a", ExpressionFieldPath::create(expCtx, "missing")}});
    ASSERT_VALUE_EQ(Value(Document{}), object->evaluate(Document()));

    auto objectWithNestedObject = ExpressionObject::create(expCtx, {{"nested", object}});
    ASSERT_VALUE_EQ(Value(Document{{"nested", Document{}}}),
                    objectWithNestedObject->evaluate(Document()));
}