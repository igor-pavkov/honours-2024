Value ExpressionSqrt::evaluateNumericArg(const Value& numericArg) const {
    auto checkArg = [](bool nonNegative) {
        uassert(28714, "$sqrt's argument must be greater than or equal to 0", nonNegative);
    };

    if (numericArg.getType() == NumberDecimal) {
        Decimal128 argDec = numericArg.getDecimal();
        checkArg(!argDec.isLess(Decimal128::kNormalizedZero));  // NaN returns Nan without error
        return Value(argDec.squareRoot());
    }
    double argDouble = numericArg.coerceToDouble();
    checkArg(!(argDouble < 0));  // NaN returns Nan without error
    return Value(sqrt(argDouble));
}