TEST_F(ExpressionCeilTest, DecimalArg) {
    assertEvaluates(Value(Decimal128("2")), Value(Decimal128("2.0")));
    assertEvaluates(Value(Decimal128("-2")), Value(Decimal128("-2.0")));
    assertEvaluates(Value(Decimal128("0.9")), Value(Decimal128("1.0")));
    assertEvaluates(Value(Decimal128("0.1")), Value(Decimal128("1.0")));
    assertEvaluates(Value(Decimal128("-1.2")), Value(Decimal128("-1.0")));
    assertEvaluates(Value(Decimal128("-1.7")), Value(Decimal128("-1.0")));
    assertEvaluates(Value(Decimal128("1234567889.000000000000000000000001")),
                    Value(Decimal128("1234567890")));
    assertEvaluates(Value(Decimal128("-99999999999999999999999999999.99")),
                    Value(Decimal128("-99999999999999999999999999999.00")));
    assertEvaluates(Value(Decimal128("3.4E-6000")), Value(Decimal128("1")));
}