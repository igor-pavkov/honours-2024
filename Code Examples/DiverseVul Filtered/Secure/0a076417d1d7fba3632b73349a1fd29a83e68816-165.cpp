TEST(BuiltinRemoveVariableTest, TypeOfRemoveIsMissing) {
    assertExpectedResults("$type", {{{Value("$$REMOVE"_sd)}, Value("missing"_sd)}});
}