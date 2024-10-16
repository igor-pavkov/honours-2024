TEST(ExpressionFromAccumulators, Avg) {
    assertExpectedResults("$avg",
                          {// $avg ignores non-numeric inputs.
                           {{Value("string"_sd), Value(BSONNULL), Value(), Value(3)}, Value(3.0)},
                           // $avg always returns a double.
                           {{Value(10LL), Value(20LL)}, Value(15.0)},
                           // $avg returns null when no arguments are provided.
                           {{}, Value(BSONNULL)}});
}