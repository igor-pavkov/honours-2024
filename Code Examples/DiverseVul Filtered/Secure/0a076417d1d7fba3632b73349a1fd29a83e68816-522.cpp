Value ExpressionFloor::evaluateNumericArg(const Value& numericArg) const {
    // There's no point in taking the floor of integers or longs, it will have no effect.
    switch (numericArg.getType()) {
        case NumberDouble:
            return Value(std::floor(numericArg.getDouble()));
        case NumberDecimal:
            // Round toward the nearest decimal with a zero exponent in the negative direction.
            return Value(numericArg.getDecimal().quantize(Decimal128::kNormalizedZero,
                                                          Decimal128::kRoundTowardNegative));
        default:
            return numericArg;
    }
}