void ExpressionFilter::_doAddDependencies(DepsTracker* deps) const {
    _input->addDependencies(deps);
    _filter->addDependencies(deps);
}