void ExpressionMap::_doAddDependencies(DepsTracker* deps) const {
    _input->addDependencies(deps);
    _each->addDependencies(deps);
}