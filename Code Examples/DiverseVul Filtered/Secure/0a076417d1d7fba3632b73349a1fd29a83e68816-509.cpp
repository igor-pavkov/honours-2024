TEST(ExpressionStrLenCP, ComputesLengthOfEmptyString) {
    assertExpectedResults("$strLenCP", {{{Value(StringData())}, Value(0)}});
}