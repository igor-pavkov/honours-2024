TEST_F(ExpressionFloorTest, NullArg) {
    assertEvaluates(Value(BSONNULL), Value(BSONNULL));
}