TEST(ExpressionObjectEvaluate, ShouldEvaluateFieldsWithinNestedObject) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto object = ExpressionObject::create(
        expCtx,
        {{"a",
          ExpressionObject::create(
              expCtx,
              {{"b", makeConstant(1)}, {"c", ExpressionFieldPath::create(expCtx, "_id")}})}});
    ASSERT_VALUE_EQ(Value(Document{{"a", Document{{"b", 1}}}}), object->evaluate(Document()));
    ASSERT_VALUE_EQ(Value(Document{{"a", Document{{"b", 1}, {"c", "ID"_sd}}}}),
                    object->evaluate(Document{{"_id", "ID"_sd}}));
}