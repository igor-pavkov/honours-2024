intrusive_ptr<ExpressionObject> ExpressionObject::parse(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONObj obj,
    const VariablesParseState& vps) {
    // Make sure we don't have any duplicate field names.
    stdx::unordered_set<string> specifiedFields;

    vector<pair<string, intrusive_ptr<Expression>>> expressions;
    for (auto&& elem : obj) {
        // Make sure this element has a valid field name. Use StringData here so that we can detect
        // if the field name contains a null byte.
        FieldPath::uassertValidFieldName(elem.fieldNameStringData());

        auto fieldName = elem.fieldName();
        uassert(16406,
                str::stream() << "duplicate field name specified in object literal: "
                              << obj.toString(),
                specifiedFields.find(fieldName) == specifiedFields.end());
        specifiedFields.insert(fieldName);
        expressions.emplace_back(fieldName, parseOperand(expCtx, elem, vps));
    }

    return new ExpressionObject{expCtx, std::move(expressions)};
}