Value ExpressionTrunc::evaluateNumericArg(const Value& numericArg) const {
    // There's no point in truncating integers or longs, it will have no effect.
    switch (numericArg.getType()) {
        case NumberDecimal:
            return Value(numericArg.getDecimal().quantize(Decimal128::kNormalizedZero,
                                                          Decimal128::kRoundTowardZero));
        case NumberDouble:
            return Value(std::trunc(numericArg.getDouble()));
        default:
            return numericArg;
    }
}