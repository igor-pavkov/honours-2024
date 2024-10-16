TEST(ExpressionStrLenCP, ComputesLengthOfASCIIString) {
    assertExpectedResults("$strLenCP", {{{Value("abc"_sd)}, Value(3)}});
}