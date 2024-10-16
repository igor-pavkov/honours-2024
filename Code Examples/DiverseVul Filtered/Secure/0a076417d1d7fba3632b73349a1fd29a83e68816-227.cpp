TEST(ExpressionObjectDependencies, ConstantValuesShouldNotBeAddedToDependencies) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto object = ExpressionObject::create(expCtx, {{"a", makeConstant(5)}});
    DepsTracker deps;
    object->addDependencies(&deps);
    ASSERT_EQ(deps.fields.size(), 0UL);
}