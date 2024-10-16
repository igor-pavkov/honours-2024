TEST_F(ExpressionTruncTest, DecimalArg) {
    assertEvaluates(Value(Decimal128("2")), Value(Decimal128("2.0")));
    assertEvaluates(Value(Decimal128("-2")), Value(Decimal128("-2.0")));
    assertEvaluates(Value(Decimal128("0.9")), Value(Decimal128("0.0")));
    assertEvaluates(Value(Decimal128("0.1")), Value(Decimal128("0.0")));
    assertEvaluates(Value(Decimal128("-1.2")), Value(Decimal128("-1.0")));
    assertEvaluates(Value(Decimal128("-1.7")), Value(Decimal128("-1.0")));
    assertEvaluates(Value(Decimal128("123456789.9999999999999999999999999")),
                    Value(Decimal128("123456789")));
    assertEvaluates(Value(Decimal128("-99999999999999999999999999999.99")),
                    Value(Decimal128("-99999999999999999999999999999.00")));
    assertEvaluates(Value(Decimal128("3.4E-6000")), Value(Decimal128("0")));
}