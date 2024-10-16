void ExpressionLet::_doAddDependencies(DepsTracker* deps) const {
    for (auto&& idToNameExp : _variables) {
        // Add the external dependencies from the 'vars' statement.
        idToNameExp.second.expression->addDependencies(deps);
    }

    // Add subexpression dependencies, which may contain a mix of local and external variable refs.
    _subExpression->addDependencies(deps);
}