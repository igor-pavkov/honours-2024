TEST(ExpressionStrLenCP, ComputesLengthOfStringWithNull) {
    assertExpectedResults("$strLenCP", {{{Value("ab\0c"_sd)}, Value(4)}});
}