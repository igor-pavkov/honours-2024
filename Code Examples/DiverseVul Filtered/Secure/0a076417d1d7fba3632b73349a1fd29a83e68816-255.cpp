TEST(ExpressionFromAccumulators, Max) {
    assertExpectedResults("$max",
                          {// $max treats non-numeric inputs as valid arguments.
                           {{Value(1), Value(BSONNULL), Value(), Value("a"_sd)}, Value("a"_sd)},
                           {{Value("a"_sd), Value("b"_sd)}, Value("b"_sd)},
                           // $max always preserves the type of the result.
                           {{Value(10LL), Value(0.0), Value(5)}, Value(10LL)},
                           // $max returns null when no arguments are provided.
                           {{}, Value(BSONNULL)}});
}