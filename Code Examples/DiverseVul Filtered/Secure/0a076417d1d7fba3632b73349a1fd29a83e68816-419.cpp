Value ExpressionReverseArray::evaluate(const Document& root) const {
    Value input(vpOperand[0]->evaluate(root));

    if (input.nullish()) {
        return Value(BSONNULL);
    }

    uassert(34435,
            str::stream() << "The argument to $reverseArray must be an array, but was of type: "
                          << typeName(input.getType()),
            input.isArray());

    if (input.getArrayLength() < 2) {
        return input;
    }

    std::vector<Value> array = input.getArray();
    std::reverse(array.begin(), array.end());
    return Value(array);
}