TEST(ExpressionStrLenCP, ComputesLengthOfStringWithNullAtEnd) {
    assertExpectedResults("$strLenCP", {{{Value("abc\0"_sd)}, Value(4)}});
}