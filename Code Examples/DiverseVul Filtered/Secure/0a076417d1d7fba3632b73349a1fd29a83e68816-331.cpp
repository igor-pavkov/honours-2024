TEST(ExpressionObjectEvaluate, OrderOfFieldsInOutputShouldMatchOrderInSpecification) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto object = ExpressionObject::create(expCtx,
                                           {{"a", ExpressionFieldPath::create(expCtx, "a")},
                                            {"b", ExpressionFieldPath::create(expCtx, "b")},
                                            {"c", ExpressionFieldPath::create(expCtx, "c")}});
    ASSERT_VALUE_EQ(
        Value(Document{{"a", "A"_sd}, {"b", "B"_sd}, {"c", "C"_sd}}),
        object->evaluate(Document{{"c", "C"_sd}, {"a", "A"_sd}, {"b", "B"_sd}, {"_id", "ID"_sd}}));
}