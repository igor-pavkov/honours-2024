void ExpressionNary::_doAddDependencies(DepsTracker* deps) const {
    for (auto&& operand : vpOperand) {
        operand->addDependencies(deps);
    }
}