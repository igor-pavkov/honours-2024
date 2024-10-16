void uassertIfNotIntegralAndNonNegative(Value val,
                                        StringData expressionName,
                                        StringData argumentName) {
    uassert(40096,
            str::stream() << expressionName << "requires an integral " << argumentName
                          << ", found a value of type: "
                          << typeName(val.getType())
                          << ", with value: "
                          << val.toString(),
            val.integral());
    uassert(40097,
            str::stream() << expressionName << " requires a nonnegative " << argumentName
                          << ", found: "
                          << val.toString(),
            val.coerceToInt() >= 0);
}