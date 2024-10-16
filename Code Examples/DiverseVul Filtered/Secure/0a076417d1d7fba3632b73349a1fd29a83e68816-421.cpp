TEST(ExpressionObjectDependencies, FieldPathsShouldBeAddedToDependencies) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto object =
        ExpressionObject::create(expCtx, {{"x", ExpressionFieldPath::create(expCtx, "c.d")}});
    DepsTracker deps;
    object->addDependencies(&deps);
    ASSERT_EQ(deps.fields.size(), 1UL);
    ASSERT_EQ(deps.fields.count("c.d"), 1UL);
};