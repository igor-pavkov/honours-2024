TEST(ExpressionObjectParse, ShouldAcceptNestedObjects) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    auto object =
        ExpressionObject::parse(expCtx, fromjson("{a: {b: 1}, c: {d: {e: 1, f: 1}}}"), vps);
    auto expectedResult =
        Value(Document{{"a", Document{{"b", literal(1)}}},
                       {"c", Document{{"d", Document{{"e", literal(1)}, {"f", literal(1)}}}}}});
    ASSERT_VALUE_EQ(expectedResult, object->serialize(false));
}