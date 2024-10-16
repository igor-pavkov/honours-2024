TEST(ExpressionObjectParse, ShouldAcceptLiteralsAsValues) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    VariablesParseState vps = expCtx->variablesParseState;
    auto object = ExpressionObject::parse(expCtx,
                                          BSON("a" << 5 << "b"
                                                   << "string"
                                                   << "c"
                                                   << BSONNULL),
                                          vps);
    auto expectedResult =
        Value(Document{{"a", literal(5)}, {"b", literal("string"_sd)}, {"c", literal(BSONNULL)}});
    ASSERT_VALUE_EQ(expectedResult, object->serialize(false));
}