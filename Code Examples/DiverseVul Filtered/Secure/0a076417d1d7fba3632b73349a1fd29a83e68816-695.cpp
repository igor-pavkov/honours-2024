TEST(ExpressionFromAccumulators, StdDevPop) {
    assertExpectedResults("$stdDevPop",
                          {// $stdDevPop ignores non-numeric inputs.
                           {{Value("string"_sd), Value(BSONNULL), Value(), Value(3)}, Value(0.0)},
                           // $stdDevPop always returns a double.
                           {{Value(1LL), Value(3LL)}, Value(1.0)},
                           // $stdDevPop returns null when no arguments are provided.
                           {{}, Value(BSONNULL)}});
}