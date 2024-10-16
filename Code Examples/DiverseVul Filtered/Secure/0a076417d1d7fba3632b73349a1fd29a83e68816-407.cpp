Value ExpressionAllElementsTrue::evaluate(const Document& root) const {
    const Value arr = vpOperand[0]->evaluate(root);
    uassert(17040,
            str::stream() << getOpName() << "'s argument must be an array, but is "
                          << typeName(arr.getType()),
            arr.isArray());
    const vector<Value>& array = arr.getArray();
    for (vector<Value>::const_iterator it = array.begin(); it != array.end(); ++it) {
        if (!it->coerceToBool()) {
            return Value(false);
        }
    }
    return Value(true);
}