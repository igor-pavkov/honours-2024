TEST(ExpressionSubstrCPTest, WithNullCharacter) {
    assertExpectedResults("$substrCP",
                          {{{Value("abc\0d"_sd), Value(2), Value(3)}, Value("c\0d"_sd)}});
}