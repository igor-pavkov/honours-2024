Value ExpressionLn::evaluateNumericArg(const Value& numericArg) const {
    if (numericArg.getType() == NumberDecimal) {
        Decimal128 argDecimal = numericArg.getDecimal();
        if (argDecimal.isGreater(Decimal128::kNormalizedZero))
            return Value(argDecimal.logarithm());
        // Fall through for error case.
    }
    double argDouble = numericArg.coerceToDouble();
    uassert(28766,
            str::stream() << "$ln's argument must be a positive number, but is " << argDouble,
            argDouble > 0 || std::isnan(argDouble));
    return Value(std::log(argDouble));
}