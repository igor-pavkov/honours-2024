intrusive_ptr<Expression> ExpressionLet::optimize() {
    if (_variables.empty()) {
        // we aren't binding any variables so just return the subexpression
        return _subExpression->optimize();
    }

    for (VariableMap::iterator it = _variables.begin(), end = _variables.end(); it != end; ++it) {
        it->second.expression = it->second.expression->optimize();
    }

    _subExpression = _subExpression->optimize();

    return this;
}