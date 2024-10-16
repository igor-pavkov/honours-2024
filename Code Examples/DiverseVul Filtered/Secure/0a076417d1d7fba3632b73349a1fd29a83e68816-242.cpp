TEST(ExpressionFromAccumulators, StdDevSamp) {
    assertExpectedResults(
        "$stdDevSamp",
        {// $stdDevSamp ignores non-numeric inputs.
         {{Value("string"_sd), Value(BSONNULL), Value(), Value(3)}, Value(BSONNULL)},
         // $stdDevSamp always returns a double.
         {{Value(1LL), Value(2LL), Value(3LL)}, Value(1.0)},
         // $stdDevSamp returns null when no arguments are provided.
         {{}, Value(BSONNULL)}});
}