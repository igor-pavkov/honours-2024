Value ExpressionCeil::evaluateNumericArg(const Value& numericArg) const {
    // There's no point in taking the ceiling of integers or longs, it will have no effect.
    switch (numericArg.getType()) {
        case NumberDouble:
            return Value(std::ceil(numericArg.getDouble()));
        case NumberDecimal:
            // Round toward the nearest decimal with a zero exponent in the positive direction.
            return Value(numericArg.getDecimal().quantize(Decimal128::kNormalizedZero,
                                                          Decimal128::kRoundTowardPositive));
        default:
            return numericArg;
    }
}