TEST(ExpressionFromAccumulators, Sum) {
    assertExpectedResults(
        "$sum",
        {// $sum ignores non-numeric inputs.
         {{Value("string"_sd), Value(BSONNULL), Value(), Value(3)}, Value(3)},
         // If any argument is a double, $sum returns a double
         {{Value(10LL), Value(10.0)}, Value(20.0)},
         // If no arguments are doubles and an argument is a long, $sum returns a long
         {{Value(10LL), Value(10)}, Value(20LL)},
         // $sum returns 0 when no arguments are provided.
         {{}, Value(0)}});
}