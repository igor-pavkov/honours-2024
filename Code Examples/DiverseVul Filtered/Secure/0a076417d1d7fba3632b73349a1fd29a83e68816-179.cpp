Value ExpressionObject::evaluate(const Document& root) const {
    MutableDocument outputDoc;
    for (auto&& pair : _expressions) {
        outputDoc.addField(pair.first, pair.second->evaluate(root));
    }
    return outputDoc.freezeToValue();
}