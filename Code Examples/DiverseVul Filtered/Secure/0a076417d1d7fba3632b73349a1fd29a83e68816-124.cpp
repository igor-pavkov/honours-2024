TEST(ExpressionSubstrCPTest, WithNullCharacterAtEnd) {
    assertExpectedResults("$substrCP",
                          {{{Value("abc\0"_sd), Value(2), Value(2)}, Value("c\0"_sd)}});
}