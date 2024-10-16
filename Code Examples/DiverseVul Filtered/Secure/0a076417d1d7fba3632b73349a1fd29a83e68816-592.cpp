Value ExpressionZip::serialize(bool explain) const {
    vector<Value> serializedInput;
    vector<Value> serializedDefaults;
    Value serializedUseLongestLength = Value(_useLongestLength);

    for (auto&& expr : _inputs) {
        serializedInput.push_back(expr->serialize(explain));
    }

    for (auto&& expr : _defaults) {
        serializedDefaults.push_back(expr->serialize(explain));
    }

    return Value(DOC("$zip" << DOC("inputs" << Value(serializedInput) << "defaults"
                                            << Value(serializedDefaults)
                                            << "useLongestLength"
                                            << serializedUseLongestLength)));
}