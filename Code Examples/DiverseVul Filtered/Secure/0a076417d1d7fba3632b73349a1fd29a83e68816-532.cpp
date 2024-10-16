Value ExpressionSwitch::evaluate(const Document& root) const {
    for (auto&& branch : _branches) {
        Value caseExpression(branch.first->evaluate(root));

        if (caseExpression.coerceToBool()) {
            return branch.second->evaluate(root);
        }
    }

    uassert(40066,
            "$switch could not find a matching branch for an input, and no default was specified.",
            _default);

    return _default->evaluate(root);
}