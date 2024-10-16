TEST(ExpressionStrLenCP, ComputesLengthOfStringWithSpecialCharacters) {
    assertExpectedResults("$strLenCP", {{{Value("ºabøåß"_sd)}, Value(6)}});
}