Value ExpressionDivide::evaluate(const Document& root) const {
    Value lhs = vpOperand[0]->evaluate(root);
    Value rhs = vpOperand[1]->evaluate(root);

    auto assertNonZero = [](bool nonZero) { uassert(16608, "can't $divide by zero", nonZero); };

    if (lhs.numeric() && rhs.numeric()) {
        // If, and only if, either side is decimal, return decimal.
        if (lhs.getType() == NumberDecimal || rhs.getType() == NumberDecimal) {
            Decimal128 numer = lhs.coerceToDecimal();
            Decimal128 denom = rhs.coerceToDecimal();
            assertNonZero(!denom.isZero());
            return Value(numer.divide(denom));
        }

        double numer = lhs.coerceToDouble();
        double denom = rhs.coerceToDouble();
        assertNonZero(denom != 0.0);

        return Value(numer / denom);
    } else if (lhs.nullish() || rhs.nullish()) {
        return Value(BSONNULL);
    } else {
        uasserted(16609,
                  str::stream() << "$divide only supports numeric types, not "
                                << typeName(lhs.getType())
                                << " and "
                                << typeName(rhs.getType()));
    }
}