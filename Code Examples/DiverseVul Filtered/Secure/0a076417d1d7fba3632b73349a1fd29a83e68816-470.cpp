Value ExpressionExp::evaluateNumericArg(const Value& numericArg) const {
    // $exp always returns either a double or a decimal number, as e is irrational.
    if (numericArg.getType() == NumberDecimal)
        return Value(numericArg.coerceToDecimal().exponential());

    return Value(exp(numericArg.coerceToDouble()));
}