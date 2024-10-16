void ExpressionCoerceToBool::_doAddDependencies(DepsTracker* deps) const {
    pExpression->addDependencies(deps);
}