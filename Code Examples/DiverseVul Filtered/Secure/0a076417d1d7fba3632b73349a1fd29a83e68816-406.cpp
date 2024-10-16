TEST(ExpressionPowTest, LargeExponentValuesWithBaseOfOne) {
    assertExpectedResults(
        "$pow",
        {
            {{Value(1), Value(10)}, Value(1)},
            {{Value(1), Value(10LL)}, Value(1LL)},
            {{Value(1), Value(10000LL)}, Value(1LL)},

            {{Value(1LL), Value(10LL)}, Value(1LL)},

            // $pow may sometimes use a loop to compute a^b, so it's important to check
            // that the loop doesn't hang if a large exponent is provided.
            {{Value(1LL), Value(std::numeric_limits<long long>::max())}, Value(1LL)},
            {{Value(1LL), Value(std::numeric_limits<long long>::min())}, Value(1LL)},
        });
}