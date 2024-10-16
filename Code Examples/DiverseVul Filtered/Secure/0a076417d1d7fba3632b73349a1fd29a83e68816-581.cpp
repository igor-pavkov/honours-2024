TEST(ExpressionPowTest, LargeExponentValuesWithBaseOfZero) {
    assertExpectedResults(
        "$pow",
        {
            {{Value(0), Value(0)}, Value(1)},
            {{Value(0LL), Value(0LL)}, Value(1LL)},

            {{Value(0), Value(10)}, Value(0)},
            {{Value(0), Value(10000)}, Value(0)},

            {{Value(0LL), Value(10)}, Value(0LL)},

            // $pow may sometimes use a loop to compute a^b, so it's important to check
            // that the loop doesn't hang if a large exponent is provided.
            {{Value(0LL), Value(std::numeric_limits<long long>::max())}, Value(0LL)},
        });
}