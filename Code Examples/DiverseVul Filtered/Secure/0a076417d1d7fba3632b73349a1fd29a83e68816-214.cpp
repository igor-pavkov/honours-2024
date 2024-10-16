Value ExpressionLet::evaluate(const Document& root) const {
    for (const auto& item : _variables) {
        // It is guaranteed at parse-time that these expressions don't use the variable ids we
        // are setting
        getExpressionContext()->variables.setValue(item.first,
                                                   item.second.expression->evaluate(root));
    }

    return _subExpression->evaluate(root);
}