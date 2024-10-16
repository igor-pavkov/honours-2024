TEST_F(ExpressionTruncTest, DoubleArg) {
    assertEvaluates(Value(2.0), Value(2.0));
    assertEvaluates(Value(-2.0), Value(-2.0));
    assertEvaluates(Value(0.9), Value(0.0));
    assertEvaluates(Value(0.1), Value(0.0));
    assertEvaluates(Value(-1.2), Value(-1.0));
    assertEvaluates(Value(-1.7), Value(-1.0));

    // Outside the range of long longs (there isn't enough precision for decimals in this range, so
    // should just preserve the number).
    double largerThanLong = numeric_limits<long long>::max() * 2.0;
    assertEvaluates(Value(largerThanLong), Value(largerThanLong));
    double smallerThanLong = numeric_limits<long long>::min() * 2.0;
    assertEvaluates(Value(smallerThanLong), Value(smallerThanLong));
}