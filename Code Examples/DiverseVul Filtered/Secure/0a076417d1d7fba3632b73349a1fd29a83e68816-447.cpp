Value ExpressionConcat::evaluate(const Document& root) const {
    const size_t n = vpOperand.size();

    StringBuilder result;
    for (size_t i = 0; i < n; ++i) {
        Value val = vpOperand[i]->evaluate(root);
        if (val.nullish())
            return Value(BSONNULL);

        uassert(16702,
                str::stream() << "$concat only supports strings, not " << typeName(val.getType()),
                val.getType() == String);

        result << val.coerceToString();
    }

    return Value(result.str());
}