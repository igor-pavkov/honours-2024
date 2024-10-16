Value ExpressionRange::evaluate(const Document& root) const {
    Value startVal(vpOperand[0]->evaluate(root));
    Value endVal(vpOperand[1]->evaluate(root));

    uassert(34443,
            str::stream() << "$range requires a numeric starting value, found value of type: "
                          << typeName(startVal.getType()),
            startVal.numeric());
    uassert(34444,
            str::stream() << "$range requires a starting value that can be represented as a 32-bit "
                             "integer, found value: "
                          << startVal.toString(),
            startVal.integral());
    uassert(34445,
            str::stream() << "$range requires a numeric ending value, found value of type: "
                          << typeName(endVal.getType()),
            endVal.numeric());
    uassert(34446,
            str::stream() << "$range requires an ending value that can be represented as a 32-bit "
                             "integer, found value: "
                          << endVal.toString(),
            endVal.integral());

    int current = startVal.coerceToInt();
    int end = endVal.coerceToInt();

    int step = 1;
    if (vpOperand.size() == 3) {
        // A step was specified by the user.
        Value stepVal(vpOperand[2]->evaluate(root));

        uassert(34447,
                str::stream() << "$range requires a numeric step value, found value of type:"
                              << typeName(stepVal.getType()),
                stepVal.numeric());
        uassert(34448,
                str::stream() << "$range requires a step value that can be represented as a 32-bit "
                                 "integer, found value: "
                              << stepVal.toString(),
                stepVal.integral());
        step = stepVal.coerceToInt();

        uassert(34449, "$range requires a non-zero step value", step != 0);
    }

    std::vector<Value> output;

    while ((step > 0 ? current < end : current > end)) {
        output.push_back(Value(current));
        current += step;
    }

    return Value(output);
}