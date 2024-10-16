TEST(ExpressionObjectOptimizations, OptimizingAnObjectShouldOptimizeSubExpressions) {
    // Build up the object {a: {$add: [1, 2]}}.
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    auto addExpression =
        ExpressionAdd::parse(expCtx, BSON("$add" << BSON_ARRAY(1 << 2)).firstElement(), vps);
    auto object = ExpressionObject::create(expCtx, {{"a", addExpression}});
    ASSERT_EQ(object->getChildExpressions().size(), 1UL);

    auto optimized = object->optimize();
    auto optimizedObject = dynamic_cast<ExpressionObject*>(optimized.get());
    ASSERT_TRUE(optimizedObject);
    ASSERT_EQ(optimizedObject->getChildExpressions().size(), 1UL);

    // We should have optimized {$add: [1, 2]} to just the constant 3.
    auto expConstant =
        dynamic_cast<ExpressionConstant*>(optimizedObject->getChildExpressions()[0].second.get());
    ASSERT_TRUE(expConstant);
    ASSERT_VALUE_EQ(expConstant->evaluate(Document()), Value(3));
};