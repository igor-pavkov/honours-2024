Value ExpressionObject::serialize(bool explain) const {
    MutableDocument outputDoc;
    for (auto&& pair : _expressions) {
        outputDoc.addField(pair.first, pair.second->serialize(explain));
    }
    return outputDoc.freezeToValue();
}