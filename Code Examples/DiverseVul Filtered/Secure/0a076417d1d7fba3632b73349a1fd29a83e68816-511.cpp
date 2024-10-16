TEST_F(ExpressionTruncTest, NullArg) {
    assertEvaluates(Value(BSONNULL), Value(BSONNULL));
}