void ExpressionReduce::_doAddDependencies(DepsTracker* deps) const {
    _input->addDependencies(deps);
    _initial->addDependencies(deps);
    _in->addDependencies(deps);
}