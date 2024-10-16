Value ExpressionObjectToArray::evaluate(const Document& root) const {
    const Value targetVal = vpOperand[0]->evaluate(root);

    if (targetVal.nullish()) {
        return Value(BSONNULL);
    }

    uassert(40390,
            str::stream() << "$objectToArray requires a document input, found: "
                          << typeName(targetVal.getType()),
            (targetVal.getType() == BSONType::Object));

    vector<Value> output;

    FieldIterator iter = targetVal.getDocument().fieldIterator();
    while (iter.more()) {
        Document::FieldPair pair = iter.next();
        MutableDocument keyvalue;
        keyvalue.addField("k", Value(pair.first));
        keyvalue.addField("v", pair.second);
        output.push_back(keyvalue.freezeToValue());
    }

    return Value(output);
}