TEST_F(ExpressionCeilTest, NullArg) {
    assertEvaluates(Value(BSONNULL), Value(BSONNULL));
}