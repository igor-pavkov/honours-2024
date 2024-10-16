TEST(ExpressionStrLenBytes, ComputesLengthOfString) {
    assertExpectedResults("$strLenBytes", {{{Value("abc"_sd)}, Value(3)}});
}