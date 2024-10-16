Value ExpressionConcatArrays::evaluate(const Document& root) const {
    const size_t n = vpOperand.size();
    vector<Value> values;

    for (size_t i = 0; i < n; ++i) {
        Value val = vpOperand[i]->evaluate(root);
        if (val.nullish()) {
            return Value(BSONNULL);
        }

        uassert(28664,
                str::stream() << "$concatArrays only supports arrays, not "
                              << typeName(val.getType()),
                val.isArray());

        const auto& subValues = val.getArray();
        values.insert(values.end(), subValues.begin(), subValues.end());
    }
    return Value(std::move(values));
}