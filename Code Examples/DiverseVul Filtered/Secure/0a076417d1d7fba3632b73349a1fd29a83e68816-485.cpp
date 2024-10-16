void ExpressionDateToString::_doAddDependencies(DepsTracker* deps) const {
    _date->addDependencies(deps);
    if (_timeZone) {
        _timeZone->addDependencies(deps);
    }
}