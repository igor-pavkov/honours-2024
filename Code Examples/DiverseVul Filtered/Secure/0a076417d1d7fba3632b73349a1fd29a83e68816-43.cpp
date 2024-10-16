TEST(ExpressionStrLenCP, ComputesLengthOfStringWithNullAtEnd) {
    assertExpectedResults("$strLenBytes", {{{Value("abc\0"_sd)}, Value(4)}});
}