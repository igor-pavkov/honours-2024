TEST_F(ExpressionFloorTest, LongArg) {
    assertEvaluates(Value(0LL), Value(0LL));
    assertEvaluates(Value(numeric_limits<long long>::min()),
                    Value(numeric_limits<long long>::min()));
    assertEvaluates(Value(numeric_limits<long long>::max()),
                    Value(numeric_limits<long long>::max()));
}